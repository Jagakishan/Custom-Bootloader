#!/usr/bin/env python3
import sys
import os


def bin_to_c_array(input_file, output_file, array_name="ota_image_bin"):
    """Convert binary file to C array for STM32 bootloader"""

    if not os.path.exists(input_file):
        print(f"Error: {input_file} not found!")
        return False

    with open(input_file, 'rb') as f:
        data = f.read()

    print(f"Converting {len(data)} bytes from {input_file}")

    # Generate C array
    c_content = f"// Auto-generated from {input_file}\n"
    c_content += f"// Size: {len(data)} bytes\n\n"
    c_content += f'const uint8_t {array_name}[] __attribute__((section(".rodata")))) = {{\n'

    # Format: 12 bytes per line, 0xHH format
    for i in range(0, len(data), 12):
        line = "  "
        chunk = data[i:i + 12]
        line += ", ".join(f"0x{b:02x}" for b in chunk)
        if i + 12 < len(data):
            line += ","
        c_content += line + "\n"

    c_content += "};\n\n"
    c_content += f"const uint32_t {array_name}_len = {len(data)};\n"

    # Write C file
    with open(output_file, 'w') as f:
        f.write(c_content)

    print(f"Generated: {output_file}")
    return True


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python bin2c.py <app.bin> <ota_image.c>")
        print("Example: python bin2c.py App1.bin ota_image.c")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    if bin_to_c_array(input_file, output_file):
        print("SUCCESS! Copy ota_image.c to bootloader/src/")
    else:
        print("FAILED!")
        sys.exit(1)
