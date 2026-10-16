#!/usr/bin/env python3
"""
Bob's Game sprite codec — .bin <-> PNG

Format (from src/engine/main/sprites.cpp + DS hardware):
- .bin = 1 byte per pixel, palette indices, NO header.
  Stored as 8x8 TILES (DS native), not row-major pixels.
  Each frame is (fw/8)*(fh/8) tiles, each tile 64 bytes row-major within the tile.
  Tiles are stored row-major within the frame. Frames are sequential.
  Frame size: Yuu 32x64 (32 tiles), Bob/adults 32x80 (40 tiles).
  Direction order: DOWN=0, UP=1, LEFT=2, RIGHT=3, UPLEFT=4, UPRIGHT=5,
  DOWNRIGHT=6, DOWNLEFT=7. 8 frames per direction.
  Yuu: 64 frames (8 dirs). Bob/adults: 32 frames (4 dirs only).
  A 64-frame Bob would be 64 * 40 tiles * 64 = 163840 bytes.
- Palette .bin = 256 x uint16 LE, RGB555 (DS):
  bits 0-4=R, 5-9=G, 10-14=B, each *8 -> 0-255.
  Use hq2xpalette for hq2x sprites (correct colors).
  Index 0 = transparent.
- PNG sheet layout (what you edit): 8 frames per row, frames in order
  left-to-right, top-to-bottom. Each frame is fw x fh, row-major pixels
  (normal image). The codec converts between tiled .bin and row-major PNG.

Usage:
  decode: python3 sprite_codec.py decode <sprite.bin> <palette.bin> <frame_w> <frame_h> <out.png>
  encode: python3 sprite_codec.py encode <in.png> <palette.bin> <frame_w> <frame_h> <out.bin>

Rules for editing:
- Keep frame_w x frame_h and frame count exactly (unless adding dirs).
- Use ONLY colors from the palette. New colors snap to nearest on encode.
- Keep index 0 (transparent) transparent. PNG alpha 0 = index 0.
- Don't reorder frames. Direction order is fixed (see above).
"""
import sys, struct
from PIL import Image

def load_palette(palette_path):
    with open(palette_path, 'rb') as f:
        raw = f.read()
    assert len(raw) == 512, f"palette must be 512 bytes, got {len(raw)}"
    pal = []
    for i in range(256):
        val = struct.unpack_from('<H', raw, i*2)[0]
        r = min((val & 0x1F)*8, 255); g = min(((val>>5)&0x1F)*8, 255); b = min(((val>>10)&0x1F)*8, 255)
        pal.append((r,g,b))
    return pal

def decode(bin_path, palette_path, fw, fh, out_png):
    pal = load_palette(palette_path)
    with open(bin_path, 'rb') as f:
        data = f.read()
    tiles_x, tiles_y = fw//8, fh//8
    assert fw % 8 == 0 and fh % 8 == 0, "frame must be multiple of 8 (tiles)"
    bytes_per_frame = tiles_x*tiles_y*64
    assert len(data) % bytes_per_frame == 0, f"file size {len(data)} not multiple of frame {bytes_per_frame}"
    nframes = len(data) // bytes_per_frame
    cols = 8
    rows = (nframes + cols - 1)//cols
    img = Image.new('RGBA', (fw*cols, fh*rows), (0,0,0,0))
    px = img.load()
    for fi in range(nframes):
        fbase = fi*bytes_per_frame
        ox = (fi % cols)*fw
        oy = (fi // cols)*fh
        for ty in range(tiles_y):
            for tx in range(tiles_x):
                tbase = fbase + (ty*tiles_x + tx)*64
                for py in range(8):
                    for pxx in range(8):
                        idx = data[tbase + py*8 + pxx]
                        if idx == 0: continue
                        r,g,b = pal[idx]
                        px[ox+tx*8+pxx, oy+ty*8+py] = (r,g,b,255)
    img.save(out_png)
    print(f"decoded {bin_path}: {nframes} frames {fw}x{fh} -> {out_png} ({fw*cols}x{fh*rows}, 8/row, tiled)")

def encode(png_path, palette_path, fw, fh, out_bin):
    pal = load_palette(palette_path)
    rgb_to_idx = {}
    for i in range(1, 256):
        rgb_to_idx[pal[i]] = i
    img = Image.open(png_path).convert('RGBA')
    W, H = img.size
    assert W % fw == 0 and H % fh == 0, f"PNG {W}x{H} must be multiple of frame {fw}x{fh}"
    cols, rows = W//fw, H//fh
    nframes = cols*rows
    print(f"encoding {png_path}: {nframes} frames {fw}x{fh} ({cols}x{rows} grid)")
    tiles_x, tiles_y = fw//8, fh//8
    bytes_per_frame = tiles_x*tiles_y*64
    out = bytearray()
    snapped = 0
    px = img.load()
    for fi in range(nframes):
        ox = (fi % cols)*fw
        oy = (fi // cols)*fh
        for ty in range(tiles_y):
            for tx in range(tiles_x):
                for py in range(8):
                    for pxx in range(8):
                        r,g,b,a = px[ox+tx*8+pxx, oy+ty*8+py]
                        if a < 128:
                            out.append(0)
                            continue
                        key = (r,g,b)
                        if key in rgb_to_idx:
                            out.append(rgb_to_idx[key])
                        else:
                            best, bestd = 1, 1e18
                            for i in range(1, 256):
                                pr,pg,pb = pal[i]
                                d = (r-pr)**2 + (g-pg)**2 + (b-pb)**2
                                if d < bestd:
                                    bestd, best = d, i
                            out.append(best)
                            snapped += 1
    with open(out_bin, 'wb') as f:
        f.write(out)
    print(f"wrote {out_bin}: {len(out)} bytes")
    if snapped:
        print(f"WARNING: {snapped} pixels snapped to nearest palette color.")
    else:
        print("All pixels exact palette matches — clean round-trip.")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__); sys.exit(1)
    cmd = sys.argv[1]
    if cmd == 'decode' and len(sys.argv) == 7:
        _,_,bin_path,pal,fw,fh,out = sys.argv
        decode(bin_path, pal, int(fw), int(fh), out)
    elif cmd == 'encode' and len(sys.argv) == 7:
        _,_,png,pal,fw,fh,out = sys.argv
        encode(png, pal, int(fw), int(fh), out)
    else:
        print(__doc__); sys.exit(1)
