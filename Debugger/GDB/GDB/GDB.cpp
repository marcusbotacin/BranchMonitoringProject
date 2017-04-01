/* Marcus Botacin
 * UNICAMP - 2017
 * GDB Stub implementation
 * BranchMonitoring Project
 */

#include "GDB.h"
#include "examples.h"
#include<assert.h>

extern SOCKET ClientSocket;

VOID fill_Data(struct gdb_regs *regs)
{
	regs->cs=EXAMPLE_CS;
	regs->ds=EXAMPLE_DS;
	regs->eax=EXAMPLE_EAX;
	regs->ebp=EXAMPLE_EBP;
	regs->ebx=EXAMPLE_EBX;
	regs->ecx=EXAMPLE_ECX;
	regs->edi=EXAMPLE_EDI;
	regs->edx=EXAMPLE_EDX;
	regs->eflags=EXAMPLE_EFLAGS;
	regs->eip=EXAMPLE_EIP;
	regs->es=EXAMPLE_ES;
	regs->esi=EXAMPLE_ESI;
	regs->esp=EXAMPLE_ESP;
	regs->fs=EXAMPLE_FS;
	regs->gs=EXAMPLE_GS;
	regs->ss=EXAMPLE_SS;
}

/* I won't comment since the following codes are the same from the original source */

int hex_to_int(char ch) {
	if ('0' <= ch && ch <= '9') {
		return ch - '0';
	} else if ('a' <= ch && ch <= 'f') {
		return ch - 'a' + 10;
	} else if ('A' <= ch && ch <= 'F') {
		return ch - 'A' + 10;
	} else {
		return 0;
	}
}



char int_to_hex(unsigned val) {
	assert(val < HEX);
	if (val < NUMBER_BASE) {
		return val + '0';
	} else {
		return val - 10 + 'a';
	}
}


void write_hex_byte(char *dest, UINT8 byte) {
	dest[FIRST_PART_BYTE] = int_to_hex(byte >> BYTE_OFFSET);
	dest[SECOND_PART_BYTE] = int_to_hex(byte & BYTE_MASK);
}

void write_hex_bytes(char *dest, UINT8 *data, size_t size) {
	size_t index;
	for (index = 0; index < size; index++) {
		write_hex_byte(dest, data[index]);
		dest += 2;
	}
}

int log_getc(FILE *fp) {
	int ch = getc(fp);
	printf("get a character\n");
	if (ch == EOF) {
		fprintf(stderr, "Got EOF: exiting\n");
		exit(0);
	}
	return ch;
}

void copy_regs_to_gdb(struct gdb_regs *regs, struct gdb_regs *mcontext) {
	memset(regs, 0, sizeof(regs));
#if defined(__i386__)
	regs->eax = mcontext->eax;
	regs->ebx = mcontext->ebx;
	regs->ecx = mcontext->ecx;
	regs->edx = mcontext->edx;
	regs->esi = mcontext->esi;
	regs->edi = mcontext->edi;
	regs->esp = mcontext->esp;
	regs->ebp = mcontext->ebp;
	regs->eflags = mcontext->eflags;
	regs->eip = mcontext->eip;
#elif defined(__x86_64__)
	regs->rax = mcontext->rax;
	regs->rbx = mcontext->rbx;
	regs->rcx = mcontext->rcx;
	regs->rdx = mcontext->rdx;
	regs->rsi = mcontext->rsi;
	regs->rdi = mcontext->rdi;
	regs->rbp = mcontext->rbp;
	regs->rsp = mcontext->rsp;
	regs->r8 = mcontext->r8;
	regs->r9 = mcontext->r9;
	regs->r10 = mcontext->r10;
	regs->r11 = mcontext->r11;
	regs->r12 = mcontext->r12;
	regs->r13 = mcontext->r13;
	regs->r14 = mcontext->r14;
	regs->r15 = mcontext->r15;
	regs->eflags = mcontext->eflags;
	regs->rip = mcontext->rip;
#else
# error Unknown architecture
#endif
}

void get_packet(FILE *fp, char *buffer, int buffer_size) {
	printf("entrou get_packet\n");
	while (1) {
		/* Wait for the start character, '$', ignoring others. */
		while (1) {
			int ch = log_getc(fp);
			if (ch == '$')
				break;
			fprintf(stderr, "Unexpected char: '%c' (%i)\n", ch, ch);
		}
		printf("received start character\n");
		int count = 0;
		UINT8 checksum = 0;
		while (1) {
			if(count > buffer_size)
			{
				exit(0);
			}
			char ch = log_getc(fp);
			if (ch == '#')
				break;
			checksum += ch;
			buffer[count++] = ch;
		}
		buffer[count] = 0;
		UINT8 received_checksum = hex_to_int(log_getc(fp)) << 4;
		received_checksum += hex_to_int(log_getc(fp));
		if (received_checksum != checksum) {
			fprintf(stderr, "got bad checksum: 0x%02x != 0x%02x\n",
				received_checksum, checksum);
			putc('-', fp);
		} else {
			putc('+', fp);
		}
		fflush(fp);
		if (received_checksum == checksum) {
			fprintf(stderr, "received: '%s'\n", buffer);
			return;
		}
	}
}

void get(char request[],int size)
{
	while (1) {
		/* Wait for the start character, '$', ignoring others. */
		while (1) {
			char ch;
			recv(ClientSocket,&ch,sizeof(char),0);
			if (ch == '$')
				break;
			fprintf(stderr, "Unexpected char: '%c' (%i)\n", ch, ch);
		}
		printf("received start character\n");
		int count = 0;
		UINT8 checksum = 0;
		while (1) {
			if(count > size)
			{
				exit(0);
			}
			char ch;
			recv(ClientSocket,&ch,sizeof(char),0);
			if (ch == '#')
				break;
			checksum += ch;
			request[count++] = ch;
		}
		request[count] = 0;
		char c;
		recv(ClientSocket,&c,sizeof(char),0);
		UINT8 received_checksum = hex_to_int(c) << 4;
		recv(ClientSocket,&c,sizeof(char),0);
		received_checksum += hex_to_int(c);
		if (received_checksum != checksum) {
			fprintf(stderr, "got bad checksum: 0x%02x != 0x%02x\n",
				received_checksum, checksum);
			c='-';
			send(ClientSocket,&c,1,0);
			//putc('-', fp);
		} else {
			printf("checksum ok\n");
			c='+';
			send(ClientSocket,&c,1,0);
			//putc('+', fp);
		}
		//fflush(fp);
		if (received_checksum == checksum) {
			fprintf(stderr, "received: '%s'\n", request);
			return;
		}
	}
}

void put_packet(FILE *fp, char *packet) {
	//putc('$', fp);
	char c;
	c='$';
	send(ClientSocket,&c,1,0);
	UINT8 checksum = 0;
	char *ptr;
	for (ptr = packet; *ptr != 0; ptr++) {
		assert(*ptr != '$');
		assert(*ptr != '#');
		//putc(*ptr, fp);
		send(ClientSocket,ptr,1,0);
		checksum += *ptr;
	}
	//putc('#', fp);
	c='#';
	send(ClientSocket,&c,1,0);
	//putc(int_to_hex(checksum >> 4), fp);
	c=int_to_hex(checksum >> 4);
	send(ClientSocket,&c,1,0);
	//putc(int_to_hex(checksum & 0xf), fp);
	c=int_to_hex(checksum & 0xf);
	send(ClientSocket,&c,1,0);
	fprintf(stderr, "sent: '%s'\n", packet);
	/* Look for acknowledgement character. */
	//int ch = log_getc(fp);
	recv(ClientSocket,&c,sizeof(char),0);

	if (c != '+') {
		fprintf(stderr, "Unexpected ack char: '%c' (%i)\n", c, c);
	}
}