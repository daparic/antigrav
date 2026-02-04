import os

def bundle_assets(assets_dir, output_file):
    files = [f for f in os.listdir(assets_dir) if f.endswith('.png')]
    files.sort()

    with open(output_file, 'w') as f:
        f.write("#ifndef ASSETS_H\n")
        f.write("#define ASSETS_H\n\n")
        f.write("#include <map>\n")
        f.write("#include <vector>\n")
        f.write("#include <string>\n\n")

        for filename in files:
            var_name = "asset_" + filename.replace('.', '_')
            filepath = os.path.join(assets_dir, filename)
            
            with open(filepath, 'rb') as asset_file:
                data = asset_file.read()
            
            f.write(f"const unsigned char {var_name}[] = {{\n")
            hex_data = [f"0x{b:02x}" for b in data]
            for i in range(0, len(hex_data), 12):
                f.write("    " + ", ".join(hex_data[i:i+12]) + ",\n")
            f.write("};\n")
            f.write(f"const unsigned int {var_name}_len = {len(data)};\n\n")

        f.write("struct Asset {\n")
        f.write("    const unsigned char* data;\n")
        f.write("    unsigned int size;\n")
        f.write("};\n\n")

        f.write("inline std::map<char, Asset> get_embedded_assets() {\n")
        f.write("    std::map<char, Asset> assets;\n")
        for filename in files:
            # filename is like 'w_p.png' or 'b_q.png'
            parts = filename.split('_')
            if len(parts) < 2:
                continue
            color = parts[0] # 'w' or 'b'
            piece = parts[1][0] # 'p', 'n', etc.
            
            piece_char = piece.upper() if color == 'w' else piece.lower()
            
            var_name = "asset_" + filename.replace('.', '_')
            f.write(f"    assets['{piece_char}'] = {{ {var_name}, {var_name}_len }};\n")
        f.write("    return assets;\n")
        f.write("}\n")

        f.write("#endif // ASSETS_H\n")

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 3:
        print("Usage: python bundle_assets.py <assets_dir> <output_file>")
        sys.exit(1)
    bundle_assets(sys.argv[1], sys.argv[2])
