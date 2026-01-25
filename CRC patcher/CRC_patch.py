import zlib
import struct

BIN_FILE = r"C:\Users\Jagakishan\STM32CubeIDE\workspace_1.18.0\dummyBlinkForDebug\Debug\application.bin"
HEADER_SIZE = 0x80                              # 128 bytes
CRC_OFFSET  = 0x08                              # crc32 field
SIZE_OFFSET = 0x04                              # image_size field

with open(BIN_FILE, "rb") as f:
    data = bytearray(f.read())

if len(data) <= HEADER_SIZE:
    print("Binary smaller than header size")

app_data = data[HEADER_SIZE:]

crc  = zlib.crc32(app_data) & 0xFFFFFFFF
size = len(app_data)

print(f"CRC32      = 0x{crc:08X}")
print(f"Image size = {size} bytes")

# struct.pack_into("<I", data, SIZE_OFFSET, size)
struct.pack_into("<I", data, CRC_OFFSET, crc)

with open(BIN_FILE, "wb") as f:
    f.write(data)

print("Header patched successfully")
