#!/usr/bin/env python3
"""
Bob's Game sprite codec — .bin <-> PNG

Format (from src/engine/main/sprites.cpp):
- .bin = raw 1 byte per pixel, palette indices, no header, row-major.
  Frames stored sequentially: frame 0, frame 1, ... Each frame is
  frame_w x frame_h pixels. Direction order: DOWN=0, UP=1, LEFT=2, RIGHT=3,
  UPLEFT=4, UPRIGHT=5, DOWNRIGHT=6, DOWNLEFT=7. 8 frames per direction.
  Yuu: 64 frames (32x64). Bob/adults: 32 frames (32x80) — 4 dirs only.
  A 64-frame Bob would be 64 * 32 * 80 = 163840 bytes.
- Palette .bin = 256 x uint16 LE, RGB555 (DS format):
  bits 0-4=R, 5-9=G, 10-14=B, each *8 -> 0-255.
  Index 0 = transparent.
- PNG sheet layout: 8 frames per row, frames in order left-to-right,
  top-to-bottom. Each frame is frame_w x frame_h.

Usage:
  decode: python3 sprite_codec.py decode <sprite.bin> <palette.bin> <frame_w> <frame_h> <out.png>
  encode: python3 sprite_codec.py encode <in.png> <palette.bin> <frame_w> <frame_h> <out.bin>

Rules for editing:
- Keep frame_w x frame_h and frame count exactly. Changing them breaks the
  animation math (gfx_index = w*h*8*dir + w*h*frame).
- Use ONLY colors from the palette. Any new color will be snapped to the
  nearest palette color on encode (and you'll get a warning count).
- Keep index 0 (transparent) transparent. In the PNG, transparent = alpha 0.
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
        r = (val & 0x1F) * 8
        g = ((val >> 5) & 0x1F) * 8
        b = ((val >> 10) & 0x1F) * 8
        r = min(r, 255); g = min(g, 255); b = min(b, 255)
        pal.append((r, g, b))
    return pal

def decode(bin_path, palette_path, fw, fh, out_png):
    pal = load_palette(palette_path)
    with open(bin_path, 'rb') as f:
        data = f.read()
    assert len(data) % (fw*fh) == 0, f"file size {len(data)} not a multiple of frame {fw}x{fh}={fw*fh}"
    nframes = len(data) // (fw*fh)
    cols = 8
    rows = (nframes + cols - 1)//cols
    img = Image.new('RGBA', (fw*cols, fh*rows), (0,0,0,0))
    px = img.load()
    for fi in range(nframes):
        base = fi*fw*fh
        ox = (fi % cols)*fw
        oy = (fi // cols)*fh
        for y in range(fh):
            for x in range(fw):
                idx = data[base + y*fw + x]
                if idx == 0:
                    continue  # transparent
                r,g,b = pal[idx]
                px[ox+x, oy+y] = (r,g,b,255)
    img.save(out_png)
    print(f"decoded {bin_path}: {nframes} frames {fw}x{fh} -> {out_png} ({fw*cols}x{fh*rows}, 8/row)")

def encode(png_path, palette_path, fw, fh, out_bin):
    pal = load_palette(palette_path)
    # Build RGB -> index map for exact matches (skip index 0)
    rgb_to_idx = {}
    for i in range(1, 256):
        rgb_to_idx[pal[i]] = i
    img = Image.open(png_path).convert('RGBA')
    W, H = img.size
    assert W % fw == 0 and H % fh == 0, f"PNG {W}x{H} must be a multiple of frame {fw}x{fh}"
    cols = W // fw
    rows = H // fh
    nframes = cols * rows
    print(f"encoding {png_path}: {nframes} frames {fw}x{fh} ({cols}x{rows} grid)")
    px = img.load()
    out = bytearray()
    snapped = 0
    for fi in range(nframes):
        ox = (fi % cols)*fw
        oy = (fi // cols)*fh
        for y in range(fh):
            for x in range(fw):
                r,g,b,a = px[ox+x, oy+y]
                if a < 128:
                    out.append(0)
                    continue
                key = (r,g,b)
                if key in rgb_to_idx:
                    out.append(rgb_to_idx[key])
                else:
                    # nearest palette color (skip index 0)
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
        print(f"WARNING: {snapped} pixels snapped to nearest palette color (not exact matches).")
        print("To avoid this, edit using only the palette colors from the decoded PNG.")
    else:
        print("All pixels exact palette matches — clean round-trip.")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)
    cmd = sys.argv[1]
    if cmd == 'decode' and len(sys.argv) == 7:
        _,_,bin_path,pal,fw,fh,out = sys.argv
        decode(bin_path, pal, int(fw), int(fh), out)
    elif cmd == 'encode' and len(sys.argv) == 7:
        _,_,png,pal,fw,fh,out = sys.argv
        encode(png, pal, int(fw), int(fh), out)
    else:
        print(__doc__)
        sys.exit(1)
