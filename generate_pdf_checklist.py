#!/usr/bin/env python3
"""
Generate PDF from TESTING_CHECKLIST.md
Requires: pip install markdown weasyprint
Or: pip install markdown pdfkit (requires wkhtmltopdf)
Or: Use online converter or print-to-PDF from browser
"""

import sys
import os
import subprocess

def check_dependencies():
    """Check if required dependencies are available"""
    try:
        import markdown
        import weasyprint
        return "weasyprint"
    except ImportError:
        pass
    
    try:
        import markdown
        import pdfkit
        # Check if wkhtmltopdf is installed
        result = subprocess.run(['which', 'wkhtmltopdf'], 
                              capture_output=True, text=True)
        if result.returncode == 0:
            return "pdfkit"
    except ImportError:
        pass
    
    return None

def generate_pdf_weasyprint(md_file, pdf_file):
    """Generate PDF using weasyprint"""
    import markdown
    from weasyprint import HTML, CSS
    
    with open(md_file, 'r', encoding='utf-8') as f:
        md_content = f.read()
    
    # Convert markdown to HTML
    html_content = markdown.markdown(md_content, extensions=['tables', 'fenced_code'])
    
    # Add CSS for better formatting
    html_with_css = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <style>
            @page {{
                size: A4;
                margin: 2cm;
            }}
            body {{
                font-family: Arial, sans-serif;
                font-size: 10pt;
                line-height: 1.4;
            }}
            h1 {{
                font-size: 18pt;
                page-break-after: avoid;
            }}
            h2 {{
                font-size: 14pt;
                page-break-after: avoid;
                margin-top: 1em;
            }}
            h3 {{
                font-size: 12pt;
                page-break-after: avoid;
            }}
            ul, ol {{
                margin: 0.5em 0;
            }}
            li {{
                margin: 0.2em 0;
            }}
            table {{
                border-collapse: collapse;
                width: 100%;
                margin: 1em 0;
            }}
            th, td {{
                border: 1px solid #ddd;
                padding: 8px;
                text-align: left;
            }}
            th {{
                background-color: #f2f2f2;
            }}
            code {{
                background-color: #f4f4f4;
                padding: 2px 4px;
                border-radius: 3px;
            }}
            pre {{
                background-color: #f4f4f4;
                padding: 10px;
                border-radius: 5px;
                overflow-x: auto;
            }}
            .test-case {{
                margin: 0.5em 0;
            }}
        </style>
    </head>
    <body>
        {html_content}
    </body>
    </html>
    """
    
    HTML(string=html_with_css).write_pdf(pdf_file)
    print(f"✓ PDF generated successfully: {pdf_file}")

def generate_pdf_pdfkit(md_file, pdf_file):
    """Generate PDF using pdfkit (requires wkhtmltopdf)"""
    import markdown
    import pdfkit
    
    with open(md_file, 'r', encoding='utf-8') as f:
        md_content = f.read()
    
    # Convert markdown to HTML
    html_content = markdown.markdown(md_content, extensions=['tables', 'fenced_code'])
    
    # Add CSS
    html_with_css = f"""
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="utf-8">
        <style>
            body {{ font-family: Arial, sans-serif; font-size: 10pt; }}
            h1 {{ font-size: 18pt; }}
            h2 {{ font-size: 14pt; margin-top: 1em; }}
            h3 {{ font-size: 12pt; }}
            ul, ol {{ margin: 0.5em 0; }}
            li {{ margin: 0.2em 0; }}
        </style>
    </head>
    <body>
        {html_content}
    </body>
    </html>
    """
    
    options = {
        'page-size': 'A4',
        'margin-top': '2cm',
        'margin-right': '2cm',
        'margin-bottom': '2cm',
        'margin-left': '2cm',
        'encoding': "UTF-8",
    }
    
    pdfkit.from_string(html_with_css, pdf_file, options=options)
    print(f"✓ PDF generated successfully: {pdf_file}")

def main():
    md_file = "TESTING_CHECKLIST.md"
    pdf_file = "TESTING_CHECKLIST.pdf"
    
    if not os.path.exists(md_file):
        print(f"Error: {md_file} not found")
        sys.exit(1)
    
    method = check_dependencies()
    
    if method == "weasyprint":
        print("Using weasyprint to generate PDF...")
        generate_pdf_weasyprint(md_file, pdf_file)
    elif method == "pdfkit":
        print("Using pdfkit to generate PDF...")
        generate_pdf_pdfkit(md_file, pdf_file)
    else:
        print("No PDF generation library found.")
        print("\nTo generate PDF, install one of the following:")
        print("  Option 1: pip install markdown weasyprint")
        print("  Option 2: pip install markdown pdfkit")
        print("           (Also requires: brew install wkhtmltopdf on macOS)")
        print("\nAlternative methods:")
        print("  1. Open TESTING_CHECKLIST.md in a markdown viewer")
        print("  2. Use 'Print to PDF' from your browser")
        print("  3. Use online converter: https://www.markdowntopdf.com/")
        print("  4. Use pandoc: pandoc TESTING_CHECKLIST.md -o TESTING_CHECKLIST.pdf")
        sys.exit(1)

if __name__ == "__main__":
    main()
