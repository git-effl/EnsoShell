import os

try:
    from PIL import Image
except ImportError:
    print("Error: Pillow is not installed. Run 'pip install Pillow' first.")
    exit(1)

# Standard Source Files in Root (excluding pic0) -> (Target Path, Width, Height, Crop Square)
ASSETS = {
    "icon0.png": ("sce_sys/icon0.png", 128, 128, True),
    "bg0.png": ("sce_sys/livearea/contents/bg0.png", 840, 500, False),
    "startup.png": ("sce_sys/livearea/contents/startup.png", 280, 158, False)
}

def process_assets():
    os.makedirs("sce_sys/livearea/contents", exist_ok=True)
    found_any = False

    for src_name, (target_path, width, height, crop_square) in ASSETS.items():
        if os.path.exists(src_name):
            found_any = True
            img = Image.open(src_name).convert("RGBA")
            
            if crop_square:
                w, h = img.size
                m = min(w, h)
                img = img.crop(((w - m) / 2, (h - m) / 2, (w + m) / 2, (h + m) / 2))
            
            img = img.resize((width, height), Image.LANCZOS)
            img.save(target_path, "PNG")
            print(f"✓ Processed {src_name} -> {target_path} ({width}x{height})")
        else:
            print(f"Skipped {src_name} (not found in root)")

    if not found_any:
        print("\nPlace standard images (icon0.png, bg0.png, startup.png) in your project root to use them.")

if __name__ == "__main__":
    process_assets()