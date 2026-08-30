#!/usr/bin/env python3
import re
import os

def minify_html(input_file, output_file):
    """Minimiert HTML-Datei für Speicheroptimierung auf dem ESP32"""

    with open(input_file, 'r', encoding='utf-8') as f:
        content = f.read()

    # Entferne HTML-Kommentare (<!-- ... -->)
    content = re.sub(r'<!--.*?-->', '', content, flags=re.DOTALL)

    # Entferne Newlines und extra Whitespace zwischen Tags
    content = re.sub(r'>\s+<', '><', content)

    # Entferne Leerzeichen in CSS
    content = re.sub(r':\s+', ':', content)
    content = re.sub(r';\s+', ';', content)
    content = re.sub(r',\s+', ',', content)
    content = re.sub(r'\s+{', '{', content)
    content = re.sub(r'{\s+', '{', content)
    content = re.sub(r'}\s+', '}', content)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(content)

    original = os.path.getsize(input_file)
    minified = os.path.getsize(output_file)
    saved = original - minified
    percent = (saved / original) * 100

    print(f"✅ Minified HTML:")
    print(f"   Original: {original:,} bytes")
    print(f"   Minified: {minified:,} bytes")
    print(f"   Saved: {saved:,} bytes ({percent:.1f}%)")

    return minified

if __name__ == '__main__':
    base_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    input_html = os.path.join(base_path, 'data', 'index.html')
    output_html = os.path.join(base_path, 'data', 'index.html.min')

    minify_html(input_html, output_html)
    print(f"\n💡 Ersetze in WebHandler.cpp:")
    print(f'   request->send(LittleFS, "/index.html.min", "text/html; charset=utf-8", false);')

