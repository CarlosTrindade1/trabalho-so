/* Author(s): <Your name(s) here>
 * Creates operating system image suitable for placement on a boot disk
*/
/* TODO: Comment on the status of your submission. Largely unimplemented */
#include <assert.h>
#include <elf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define IMAGE_FILE "./image"
#define ARGS "[--extended] <bootblock> <executable-file> ..."

#define SECTOR_SIZE 512       /* floppy sector size in bytes */
#define BOOTLOADER_SIG_OFFSET 0x1fe /* offset for boot loader signature */
// more defines...

/* Reads in an executable file in ELF format*/
Elf32_Phdr * read_exec_file(FILE **execfile, char *filename, Elf32_Ehdr **ehdr)
{ 
	FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }

	fread(*ehdr, sizeof(Elf32_Ehdr), 1, file);

	// Verifica a identificação ELF
    if ((*ehdr)->e_ident[EI_MAG0] != ELFMAG0 || (*ehdr)->e_ident[EI_MAG1] != ELFMAG1 ||
        (*ehdr)->e_ident[EI_MAG2] != ELFMAG2 || (*ehdr)->e_ident[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Arquivo não é um arquivo ELF válido.\n");
        exit(EXIT_FAILURE);
    }

	// Aloca memória para armazenar o cabeçalho do programa
    Elf32_Phdr *program_header = malloc(sizeof(Elf32_Phdr) * (*ehdr)->e_phnum);

    if (program_header == NULL) {
        perror("Erro de alocação de memória");
        exit(EXIT_FAILURE);
    }

	// Move o ponteiro do arquivo para o início da tabela de cabeçalhos de programas
    fseek(file, (*ehdr)->e_phoff, SEEK_SET);

    // Le os cabeçalhos de programa
    fread(program_header, sizeof(Elf32_Phdr), (*ehdr)->e_phnum, file);

    fclose(file);

    return program_header;
}

void writeZerosAndSignature(FILE *imagefile) {
    long current_offset = ftell(imagefile);

    // calcula quantos bytes de zero devem ser adicionados
    long zeros_to_add = 0x200 - current_offset;

    // escreve zeros até o offset desejado
    for (long i = 0; i < zeros_to_add - 2; i++) {
        fputc(0, imagefile);
    }

    //escreve assinatura específica
    unsigned char aa55[] = { 0x55, 0xAA };
    fwrite(aa55, 1, sizeof(aa55), imagefile);
}

/* Writes the bootblock to the image file */
void write_bootblock(FILE **imagefile,FILE *bootfile,Elf32_Ehdr *boot_header, Elf32_Phdr *boot_phdr)
{
	fseek(bootfile, boot_phdr->p_offset, SEEK_SET);

	char *boot_data = malloc(boot_phdr->p_filesz + 1);
    if (boot_data == NULL) {
        perror("Error allocating memory");
        exit(1);
    }

	// Ler o conteúdo do segmento do bootblock do bootfile
    fread(boot_data, 1, boot_phdr->p_filesz, bootfile);
    
    // completa o ultimo byte com 00 - necessario pra inversão dar certo no ultimo byte
    boot_data[boot_phdr->p_filesz] = 0x00;

    //escreve o conteúdo do segmento do bootblock no arquivo de imagem
    fwrite(boot_data, 1, boot_phdr->p_filesz + 1, *imagefile);

    writeZerosAndSignature(*imagefile);

    // Liberar a memória alocada
    free(boot_data);
}

/* Writes the kernel to the image file */
void write_kernel(FILE **imagefile,FILE *kernelfile,Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr)
{
    fseek(kernelfile, kernel_phdr->p_offset, SEEK_SET);

    char *kernel_data = malloc(kernel_phdr->p_filesz + 1);
    if (kernel_data == NULL) {
        perror("Error allocating memory");
        exit(1);
    }

    //le conteudo do kernel
    fread(kernel_data, 1, kernel_phdr->p_filesz, kernelfile);

    //escreve o conteúdo do segmento do bootblock no arquivo de imagem
    fwrite(kernel_data, 1, kernel_phdr->p_filesz + 1, *imagefile);
}

/* Counts the number of sectors in the kernel */
int count_kernel_sectors(Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr)
{   
    return 0;
}

/* Records the number of sectors in the kernel */
void record_kernel_sectors(FILE **imagefile,Elf32_Ehdr *kernel_header, Elf32_Phdr *kernel_phdr, int num_sec)
{    
}


/* Prints segment information for --extended option */
void extended_opt(Elf32_Phdr *bph, int k_phnum, Elf32_Phdr *kph, int num_sec)
{

	/* print number of disk sectors used by the image */

  
	/* bootblock segment info */
 

	/* print kernel segment info */
  

	/* print kernel size in sectors */
}
// more helper functions...

/* MAIN */
int main(int argc, char **argv)
{
	FILE *kernelfile, *bootfile,*imagefile;  // file pointers for bootblock,kernel and image
	Elf32_Ehdr *boot_header   = malloc(sizeof(Elf32_Ehdr)); // bootblock ELF header
	Elf32_Ehdr *kernel_header = malloc(sizeof(Elf32_Ehdr)); // kernel ELF header
	
	Elf32_Phdr *boot_program_header;   // bootblock ELF program header
	Elf32_Phdr *kernel_program_header; // kernel ELF program header

	/* build image file */
	imagefile = fopen("imagem.bin", "wb");

    if (imagefile == NULL) {
        perror("Error creating image file");
        return 1;
    }

	/* read executable bootblock file */  
	bootfile = fopen(argv[1], "rb");
	boot_program_header = read_exec_file(&bootfile, argv[1], &boot_header);

	/* write bootblock */  
	write_bootblock(&imagefile, bootfile, boot_header, boot_program_header);

	/* read executable kernel file */
    kernelfile = fopen(argv[2], "rb");
	kernel_program_header = read_exec_file(&kernelfile, argv[2], &kernel_header);

	/* write kernel segments to image */
    write_kernel(&imagefile, kernelfile, kernel_header, kernel_program_header);

	/* tell the bootloader how many sectors to read to load the kernel */

	/* check for  --extended option */
	if(!strncmp(argv[1], "--extended", 11)) {
		/* print info */
	}
	
	return 0;
} // ends main()



