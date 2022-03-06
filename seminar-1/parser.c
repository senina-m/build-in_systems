#include <windows.h>
#include <stdio.h> 
#include <stdbool.h>

// or "rb", "wb", "ab", "rb+", "r+b", "wb+", "w+b", "ab+", "a+b" for binary files
FILE* file_open(const char* filename, const char* mode){
    // fprintf(stdout, "Opening file!\n");
    FILE* file = fopen(filename, mode);
    return file;
}

bool file_close(FILE* file){
    if (fclose(file) == EOF) return false;
    else return true;
}

void save_section_code(FILE* file, DWORD size_of_data, byte* pointer){
	fwrite(pointer, size_of_data, 1, file);
}

void save_entry_point(FILE* file, DWORD adress_of_entery_point){
	fprintf(file, "Entry point:\t\t0x%x\n", adress_of_entery_point);
}

void save_section_info(FILE* file, PIMAGE_SECTION_HEADER sectionHeader){
	fprintf(file, "-------- %s --------\n", sectionHeader->Name);
	fprintf(file, "Virtual Size:\t\t0x%x\n", sectionHeader->Misc.VirtualSize);
	fprintf(file, "Virtual Address:\t\t0x%x\n", sectionHeader->VirtualAddress);
	fprintf(file, "Size Of Raw Data:\t\t0x%x\n", sectionHeader->SizeOfRawData);
	fprintf(file, "Pointer To Raw Data:\t\t0x%x\n", sectionHeader->PointerToRawData);
	fprintf(file, "Pointer To Relocations:\t\t0x%x\n", sectionHeader->PointerToRelocations);
	fprintf(file, "Pointer To Line Numbers:\t\t0x%x\n", sectionHeader->PointerToLinenumbers);
	fprintf(file, "Number Of Relocations:\t\t0x%x\n", sectionHeader->NumberOfRelocations);
	fprintf(file, "Number Of Line Numbers:\t\t0x%x\n", sectionHeader->NumberOfLinenumbers);
	fprintf(file, "Characteristics:\t\t0x%x\n\n", sectionHeader->Characteristics);
}

int main(int argc, char* argv[]){
    if (argc != 4) {
		printf("Enter 3 arguments -- filepath of 32bit file, filepath to code file, filepath to sections info file.");
		return 1;
	}

    HANDLE hFile;
    HANDLE hFileMapping;
    LPVOID lpFileBase;
    PIMAGE_DOS_HEADER dosHeader;
    PIMAGE_NT_HEADERS peHeader;
    PIMAGE_SECTION_HEADER sectionHeader;

    hFile = CreateFile(argv[1],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

    if(hFile==INVALID_HANDLE_VALUE){
        printf("\n CreateFile failed \n");
        return 1;
    }

    hFileMapping = CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);

    if(hFileMapping==0){
        printf("\n CreateFileMapping failed \n");
        CloseHandle(hFile);
        return 1;
    }
    lpFileBase = MapViewOfFile(hFileMapping,FILE_MAP_READ,0,0,0);

    if(lpFileBase==0){
        printf("\n MapViewOfFile failed \n");
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        return 1;
    }

    FILE* code_file = file_open(argv[2], "wb");
    FILE* info_file = file_open(argv[3], "wb");

    dosHeader = (PIMAGE_DOS_HEADER) lpFileBase;
    if(dosHeader->e_magic==IMAGE_DOS_SIGNATURE){ //check magic number
        peHeader = (PIMAGE_NT_HEADERS) ((u_char*)dosHeader+dosHeader->e_lfanew);
        if(peHeader->Signature == IMAGE_NT_SIGNATURE){ //check signature
            //once found valid exe or dll

            //go to first section
            sectionHeader = IMAGE_FIRST_SECTION(peHeader);
            UINT number_of_sections = peHeader->FileHeader.NumberOfSections;


            save_entry_point(info_file, peHeader->OptionalHeader.AddressOfEntryPoint);

            //No of sections
            printf("\n No of sections : %d \n", number_of_sections);

            //sectionHeader contains address of first section
            //traverse each section by below way
            for( UINT i=0; i< number_of_sections; ++i, ++sectionHeader ){
                printf("Section name\t\t%s\n", sectionHeader->Name);
                if(!(sectionHeader->Characteristics & IMAGE_SCN_CNT_CODE)) {
                    save_section_code(code_file, sectionHeader->SizeOfRawData, (unsigned char*) dosHeader + (sectionHeader->PointerToRawData));
                }

                save_section_info(info_file, sectionHeader);
            }
        } else return 1;
    } else return 1;

    file_close(code_file);
    file_close(info_file);
    return 0;
}