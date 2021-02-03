#include "fs.h"
#include "error.h"
#include "dop.h"
#include <stdio.h>
#include <windows.h>

//����򵥵ķ�ʽʵ�ֶԷ������ļ��Ĳ�����
//��������512�ֽ�

#define JOB_CREATE_VHD		0x00000001
#define JOB_OPEN_VHD		0x00000002
#define JOB_FORMAT_DISK		0x00000004
#define JOB_FORMAT_ESP		0x00000008
#define JOB_FORMAT_FAT		0x00000010
#define JOB_FORMAT_LFS		0x00000020
#define JOB_COPY_MBR		0x00000040
#define JOB_COPY_ESP_BOOT	0x00000080
#define JOB_COPY_FAT_BOOT	0x00000100
#define JOB_COPY_LFS_BOOT	0x00000200

/*
-C Name sector_count		Create a VHD file
-c DName SName				Copy a file into VHD
-F							Create/Recrreate partition table
-f fat/esp/lfs				Format partition
-O Name						Open a VHD file
-H							Get help
-M Name						Replace MBR
-m fat/esp/lfs Name			Replace partition's boot sector
*/

void ShowHelp() {
	wprintf(L"DOP Help:\n");
	wprintf(L"-C Name Sector_Count          Create a VHD file.\n");
	wprintf(L"-O Name                       Open a VHD file.\n");
	wprintf(L"    -C & -O command can not use toughter.\n");
	wprintf(L"-F                            Create/Recreate partition table.\n");
	wprintf(L"-f fat/esp/lfs                Format FAT/ESP/LFS partiton.\n");
	wprintf(L"-H                            Get help.\n");
	wprintf(L"-M Name                       Replace MBR(only 446 bytes).\n");
	wprintf(L"-m FAT/ESP/LFS Name           Replace partition's boot sector(s).\n");
	wprintf(L"    FAT12/FAT16 partition not include first 62 bytes.\n");
	wprintf(L"    FAT32 partition not include first 90 bytes and information sector, and backup boot sector.\n");
	wprintf(L"    LFS partition only permit 4096 bytes.\n");
	wprintf(L"-c DName SName                Copy file into VHD.\n");
}
int wmain(int argc, wchar_t ** argv, wchar_t ** envp) {
	int i;
	int Jobs = 0;
	int64_t VHDSize;
	uint32_t file_size;
	FILE * sfile;
	LPDOPSTREAM dfile;
	const wchar_t* VHDPath = NULL,* MBRPath = NULL, * ESPBoot = NULL, * FATBoot = NULL, * LFSBoot = NULL;
	HANDLE vhd,disk,lfs,fat,esp;
	struct _BUFFER_HEAD_* bh;
	char tmp_data[512];
	HANDLE lib;

	if (argc <= 1) {
		ShowHelp();
		return -1;
	}
	i = 1;
	while (i < argc) {
		if (argv[i][0] != L'-') {
			wprintf(L"Argument %d <%s> on error.Ignore.\n", i, argv[i]);
			ShowHelp();
			return -1;
		}
		switch (argv[i][1]) {
		case L'C':
			if (i + 3 > argc) {
				ShowHelp();
				return -1;
			}
			if (Jobs & (JOB_CREATE_VHD | JOB_OPEN_VHD)) {
				ShowHelp();
				return -1;
			}
			Jobs |= JOB_CREATE_VHD;
			VHDPath = argv[i + 1];
			VHDSize = wcstoll(argv[i + 2], NULL, 10);
			if (!VHDSize) {
				ShowHelp();
				return -1;
			}
			i += 3;
			break;
		case L'c':
			if (i + 3 > argc) {
				ShowHelp();
				return -1;
			}
			i += 3;
			break;
		case L'o':
		case L'O':
			if (i + 2 > argc && Jobs & (JOB_CREATE_VHD | JOB_OPEN_VHD)) {
				ShowHelp();
				return -1;
			}
			Jobs |= JOB_OPEN_VHD;
			VHDPath = argv[i + 1];
			i += 2;
			break;
		case L'F':
			Jobs |= JOB_FORMAT_DISK;
			i++;
			break;
		case L'f':
			if (i + 2 > argc) {
				ShowHelp();
				return -1;
			}
			if (((argv[i + 1][0] | 0x20) == L'l') && ((argv[i + 1][1] | 0x20) == L'f') && ((argv[i + 1][2] | 0x20) == L's')) 
				Jobs |= JOB_FORMAT_LFS;
			else if (((argv[i + 1][0] | 0x20) == L'f') && ((argv[i + 1][1] | 0x20) == L'a') && ((argv[i + 1][2] | 0x20) == L't')) 
				Jobs |= JOB_FORMAT_FAT;
			else if (((argv[i + 1][0] | 0x20) == L'e') && ((argv[i + 1][1] | 0x20) == L's') && ((argv[i + 1][2] | 0x20) == L'p')) 
				Jobs |= JOB_FORMAT_ESP;
			else {
				ShowHelp();
				return -1;
			}
			i += 2;
			break;
		case L'H':
		case L'h':
			ShowHelp();
			return -1;
		case L'M':
			if (i + 2 > argc && Jobs & JOB_COPY_MBR) {
				ShowHelp();
				return -1;
			}
			Jobs |= JOB_COPY_MBR;
			MBRPath = argv[i + 1];
			i += 2;
			break;
		case L'm':
			if (i + 3 > argc) {
				ShowHelp();
				return -1;
			}
			if (((argv[i + 1][0] | 0x20) == L'l') && ((argv[i + 1][1] | 0x20) == L'f') && ((argv[i + 1][2] | 0x20) == L's')) {
				if (Jobs & JOB_COPY_LFS_BOOT) {
					ShowHelp();
					return -1;
				}
				Jobs |= JOB_COPY_LFS_BOOT;
				LFSBoot = argv[i + 2];
			}
			else if (((argv[i + 1][0] | 0x20) == L'f') && ((argv[i + 1][1] | 0x20) == L'a') && ((argv[i + 1][2] | 0x20) == L't')) {
				if (Jobs & JOB_COPY_FAT_BOOT) {
					ShowHelp();
					return -1;
				}
				Jobs |= JOB_COPY_FAT_BOOT;
				FATBoot = argv[i + 2];
			}
			else if (((argv[i + 1][0] | 0x20) == L'e') && ((argv[i + 1][1] | 0x20) == L's') && ((argv[i + 1][2] | 0x20) == L'p')) {
				if (Jobs & JOB_COPY_ESP_BOOT) {
					ShowHelp();
					return -1;
				}
				Jobs |= JOB_COPY_ESP_BOOT;
				ESPBoot = argv[i + 2];
			}
			else {
				ShowHelp();
				return -1;
			}
			i += 3;
			break;
		default:
			wprintf(L"Argument %d <%s> on error.Ignore.\n", i, argv[i]);
			ShowHelp();
			return -1;
		}
	}
	cur_path = NULL;
	fs_init();

	lib = LoadLibraryW(L"Ole32.dll");
	if(!lib){
		wprintf(L"Can not found \"Ole32.dll\".\n");
		return -1;
	}
	CreateGuid = (int(*)(GUID*))GetProcAddress(lib,"CoCreateGuid");
	if(!CreateGuid){
		wprintf(L"Can not find \"CoCreateGuid\".\n");
		CloseHandle(lib);
		return -1;
	}

	if (Jobs & JOB_CREATE_VHD) {
		if (!(vhd = create_vhd(VHDPath, VHDSize))) {
			ShowHelp();
			CloseHandle(lib);
			return -1;
		}
		disk = summon_part_table(VHD_MAP_NAME);
		create_part_table(disk);
		esp = summon_esp();
		fat = summon_fat();
		lfs = summon_lfs();
		format_esp(esp);
		format_fat(fat);
		format_lfs(lfs);
	}
	else { 
		vhd = open_vhd(VHDPath);
		if (!vhd) {
			ShowHelp();
			printf("create vhd fail.");
			CloseHandle(lib);
			return -1;
		}
		disk = summon_part_table(VHD_MAP_NAME);
		if (!disk) {
			printf("Error 1.");
			CloseHandle(lib);
			return -1;
		}
		if (Jobs & JOB_FORMAT_DISK) { 
			create_part_table(disk);
			esp = summon_esp();
			if (!esp) {
				printf("Bug 2.\n");
				CloseHandle(lib);
				return -1;
			}
			fat = summon_fat();
			if (!fat) {
				printf("Bug 3.\n");
				CloseHandle(lib);
				return -1;
			}
			lfs = summon_lfs();
			if (!lfs) {
				printf("Bug 4.\n");
				CloseHandle(lib);
				return -1;
			}
			format_esp(esp);
			format_fat(fat);
			format_lfs(lfs);
		}
		else {
			esp = summon_esp();
			fat = summon_fat();
			lfs = summon_lfs();
			if (Jobs & JOB_FORMAT_ESP) format_esp(esp);
			if (Jobs & JOB_FORMAT_FAT) format_fat(fat);
			if (Jobs & JOB_FORMAT_LFS) format_lfs(lfs);
		}
	}
	if (Jobs & JOB_COPY_MBR) {
		dfile = open(VHD_MAP_NAME, FS_OPEN_WRITE | FS_OPEN_READ, NULL, NULL);
		if (!dfile) {
			unsummon_esp(esp);
			unsummon_fat(fat);
			unsummon_lfs(lfs);
			unsummon_part_table(disk);
			close_vhd(vhd);
			CloseHandle(lib);
			return -1;
		}
		sfile = _wfopen(MBRPath, L"rb");
		if (!sfile) {
			close(dfile);
			unsummon_esp(esp);
			unsummon_fat(fat);
			unsummon_lfs(lfs);
			unsummon_part_table(disk);
			close_vhd(vhd);
			CloseHandle(lib);
			return -1;
		}
		bh = bread(dfile, 0);
		if (!bh) {
			printf("BUG 4.\n");
			close(dfile);
			fclose(sfile);
			unsummon_esp(esp);
			unsummon_fat(fat);
			unsummon_lfs(lfs);
			unsummon_part_table(disk);
			close_vhd(vhd);
			CloseHandle(lib);
			return -1;
		}
		bdirty(bh);
		fread(bh->addr, 1, 512 - 2 - 16 * 4, sfile);
		bfree(bh);
		close(dfile);
		fclose(sfile);
	}
	if (Jobs & JOB_COPY_ESP_BOOT && esp_write_boot(esp, ESPBoot)) {
		unsummon_esp(esp);
		unsummon_fat(fat);
		unsummon_lfs(lfs);
		unsummon_part_table(disk);
		close_vhd(vhd);
		CloseHandle(lib);
		return -1;
	}
	if (Jobs & JOB_COPY_FAT_BOOT && fat_write_boot(fat, FATBoot)) {
		unsummon_esp(esp);
		unsummon_fat(fat);
		unsummon_lfs(lfs);
		unsummon_part_table(disk);
		close_vhd(vhd);
		CloseHandle(lib);
		return -1;
	}
	if (Jobs & JOB_COPY_LFS_BOOT && lfs_write_boot(lfs, LFSBoot)) {
		unsummon_esp(esp);
		unsummon_fat(fat);
		unsummon_lfs(lfs);
		unsummon_part_table(disk);
		close_vhd(vhd);
		CloseHandle(lib);
		return -1;
	}
	i = 1;
	while (i < argc) {
		if (argv[i][0] != L'-') {
			wprintf(L"Argument %d <%s> on error.Ignore.\n", i, argv[i]);
			ShowHelp();
			CloseHandle(lib);
			return -1;
		}
		switch (argv[i][1]) {
		case L'C':
			i += 3;
			break;
		case L'c':
			if (i + 3 > argc) {
				ShowHelp();
				CloseHandle(lib);
				return -1;
			}

			dfile = open(argv[i + 1], FS_OPEN_WRITE, NULL, NULL);
			if (!dfile) {
				unsummon_esp(esp);
				unsummon_fat(fat);
				unsummon_lfs(lfs);
				unsummon_part_table(disk);
				close_vhd(vhd);
				CloseHandle(lib);
				return -1;
			}
			sfile = _wfopen(argv[i + 2], L"rb");
			if (!sfile) {
				close(dfile);
				unsummon_esp(esp);
				unsummon_fat(fat);
				unsummon_lfs(lfs);
				unsummon_part_table(disk);
				close_vhd(vhd);
				CloseHandle(lib);
				return -1;
			}
			fseek(sfile, 0, SEEK_END);
			file_size = ftell(sfile);
			fseek(sfile, 0, SEEK_SET);
			while (file_size >= 512) {
				file_size -= 512;
				fread(tmp_data, 1, 512, sfile);
				write(dfile, 512, tmp_data);
			}
			if (file_size) {
				fread(tmp_data, 1, file_size, sfile);
				write(dfile, file_size, tmp_data);
			}
			close(dfile);
			fclose(sfile);
			wprintf(L"Copy file <%s> to <%s>.\n", argv[i + 2], argv[i + 1]);
			i += 3;
			break;
		case L'o':
		case L'O':
			i += 2;
			break;
		case L'F':
			i++;
			break;
		case L'f':
			i += 2;
			break;
		case L'H':
		case L'h':
			ShowHelp();
			return -1;
		case L'M':
			i += 2;
			break;
		case L'm':
			i += 3;
			break;
		default:
			wprintf(L"Argument %d <%s> on error.Ignore.\n", i, argv[i]);
			ShowHelp();
			unsummon_esp(esp);
			unsummon_fat(fat);
			unsummon_lfs(lfs);
			unsummon_part_table(disk);
			close_vhd(vhd);
			CloseHandle(lib);
			return -1;
		}
	}
	unsummon_esp(esp);
	unsummon_fat(fat);
	unsummon_lfs(lfs);
	unsummon_part_table(disk);
	close_vhd(vhd);

	CloseHandle(lib);
	return 0;
}