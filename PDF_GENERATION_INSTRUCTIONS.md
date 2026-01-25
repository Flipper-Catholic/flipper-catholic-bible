# PDF Generation Instructions

The testing checklist is available in Markdown format (`TESTING_CHECKLIST.md`). To generate a PDF for printing, use one of the following methods:

## Method 1: Python Script (Recommended)

### Install Dependencies
```bash
pip install markdown weasyprint
```

### Generate PDF
```bash
python3 generate_pdf_checklist.py
```

This will create `TESTING_CHECKLIST.pdf` in the project root.

---

## Method 2: Pandoc (If Installed)

```bash
pandoc TESTING_CHECKLIST.md -o TESTING_CHECKLIST.pdf
```

---

## Method 3: Browser Print-to-PDF

1. Open `TESTING_CHECKLIST.md` in a markdown viewer (VS Code, GitHub, etc.)
2. Use your browser's "Print" function
3. Select "Save as PDF" as the destination
4. Adjust margins and page settings as needed

---

## Method 4: Online Converter

1. Visit https://www.markdowntopdf.com/ or similar service
2. Upload `TESTING_CHECKLIST.md`
3. Download the generated PDF

---

## Method 5: VS Code Extension

1. Install "Markdown PDF" extension in VS Code
2. Open `TESTING_CHECKLIST.md`
3. Right-click → "Markdown PDF: Export (pdf)"

---

## Recommended Settings for Printing

- **Page Size:** A4 or Letter
- **Margins:** 2cm (0.8 inches)
- **Font Size:** 10pt (body), 12-18pt (headings)
- **Orientation:** Portrait
- **Print Quality:** Standard or Draft (to save ink)

---

## Notes

- The checklist is designed to fit on standard paper sizes
- Checkboxes will appear as `[ ]` in the PDF - you can check them manually
- Consider printing double-sided to save paper
- The checklist is ~157 test cases, so it will be multiple pages
