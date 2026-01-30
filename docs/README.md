# Hollow Lantern Documentation

This directory contains technical documentation for the Hollow Lantern project.

## 📚 Document Index

### Testing Documentation

#### [TESTING_GUIDE.md](TESTING_GUIDE.md)
Comprehensive guide to testing in Hollow Lantern.
- Test framework setup (Catch2)
- Writing and running tests
- Best practices and patterns
- CI/CD integration

#### [TEST_ORGANIZATION.md](TEST_ORGANIZATION.md)
Information about test structure and organization.
- Test directory layout
- Test categories and naming conventions
- How to add new tests

### Entity Data Format Documentation (NEW)

#### [ENTITY_DATA_FORMAT_SUMMARY.md](ENTITY_DATA_FORMAT_SUMMARY.md) ⭐ START HERE
Quick reference and navigation guide for entity data format conversion.
- **Best for**: Quick overview and decision making
- **Read time**: 5-10 minutes
- Problem statement
- Quick decision guide
- Document navigation
- Implementation roadmap

#### [ENTITY_DATA_FORMAT_ANALYSIS.md](ENTITY_DATA_FORMAT_ANALYSIS.md)
Comprehensive architectural analysis of data format mismatch problems.
- **Best for**: Understanding the problem and solution approaches
- **Read time**: 20-30 minutes
- Root cause analysis
- 5 solution approaches with pros/cons
- Recommendation matrix
- Best practices and migration strategy
- Testing guidelines

#### [ENTITY_DATA_CONVERTER_TEMPLATES.md](ENTITY_DATA_CONVERTER_TEMPLATES.md)
Production-ready code templates and implementation examples.
- **Best for**: Copy-paste implementation
- **Read time**: 15-20 minutes (or reference as needed)
- Complete C++23 converter module
- Test fixture update templates
- Custom matcher implementations
- Adapter pattern examples
- Error handling with `std::expected`
- Performance optimization examples

## 🎯 Quick Navigation

### "I need to understand entity data format conversion"
1. Read [ENTITY_DATA_FORMAT_SUMMARY.md](ENTITY_DATA_FORMAT_SUMMARY.md) (start here)
2. Review [ENTITY_DATA_FORMAT_ANALYSIS.md](ENTITY_DATA_FORMAT_ANALYSIS.md) for details
3. Use [ENTITY_DATA_CONVERTER_TEMPLATES.md](ENTITY_DATA_CONVERTER_TEMPLATES.md) for implementation

### "I need to implement data format conversion"
1. Check the Quick Decision Guide in [ENTITY_DATA_FORMAT_SUMMARY.md](ENTITY_DATA_FORMAT_SUMMARY.md)
2. Read the recommended approach in [ENTITY_DATA_FORMAT_ANALYSIS.md](ENTITY_DATA_FORMAT_ANALYSIS.md)
3. Copy templates from [ENTITY_DATA_CONVERTER_TEMPLATES.md](ENTITY_DATA_CONVERTER_TEMPLATES.md)

### "I need to write tests"
1. Read [TESTING_GUIDE.md](TESTING_GUIDE.md)
2. Check [TEST_ORGANIZATION.md](TEST_ORGANIZATION.md) for structure
3. Use templates from [ENTITY_DATA_CONVERTER_TEMPLATES.md](ENTITY_DATA_CONVERTER_TEMPLATES.md) if testing converters

### "I want a quick reference"
| Topic | Document | Section |
|-------|----------|---------|
| Data format conversion overview | ENTITY_DATA_FORMAT_SUMMARY.md | Quick Decision Guide |
| Which approach to use | ENTITY_DATA_FORMAT_ANALYSIS.md | Recommendation Matrix |
| Code examples | ENTITY_DATA_CONVERTER_TEMPLATES.md | All sections |
| Testing best practices | TESTING_GUIDE.md | Best Practices |
| Test organization | TEST_ORGANIZATION.md | Directory Structure |

## 📊 Document Statistics

| Document | Lines | Size | Purpose | Read Time |
|----------|-------|------|---------|-----------|
| ENTITY_DATA_FORMAT_SUMMARY.md | ~360 | 12KB | Overview & navigation | 5-10 min |
| ENTITY_DATA_FORMAT_ANALYSIS.md | ~900 | 22KB | Comprehensive analysis | 20-30 min |
| ENTITY_DATA_CONVERTER_TEMPLATES.md | ~1000 | 30KB | Code templates | 15-20 min |
| TESTING_GUIDE.md | ~300 | 9KB | Testing guide | 15-20 min |
| TEST_ORGANIZATION.md | ~280 | 8KB | Test structure | 10-15 min |

## 🔍 Document Relationships

```
ENTITY_DATA_FORMAT_SUMMARY.md (Entry Point)
├─→ ENTITY_DATA_FORMAT_ANALYSIS.md (Deep Dive)
│   └─→ ENTITY_DATA_CONVERTER_TEMPLATES.md (Implementation)
│
└─→ TESTING_GUIDE.md (Testing Practices)
    └─→ TEST_ORGANIZATION.md (Test Structure)
```

## 🎓 Learning Path

### For Beginners
1. **Start with testing basics**:
   - Read TESTING_GUIDE.md
   - Understand test organization (TEST_ORGANIZATION.md)

2. **Learn about data conversion** (when needed):
   - Start with ENTITY_DATA_FORMAT_SUMMARY.md
   - Reference ENTITY_DATA_FORMAT_ANALYSIS.md as needed
   - Copy templates from ENTITY_DATA_CONVERTER_TEMPLATES.md

### For Experienced Developers
1. **Quick reference**:
   - Use ENTITY_DATA_FORMAT_SUMMARY.md for decision making
   - Jump directly to relevant sections in analysis document
   - Grab code templates from templates document

2. **Implementation**:
   - Review approach in ENTITY_DATA_FORMAT_ANALYSIS.md
   - Copy and adapt templates from ENTITY_DATA_CONVERTER_TEMPLATES.md
   - Follow testing guidelines from TESTING_GUIDE.md

## 📝 Documentation Standards

All documentation in this directory follows these standards:

- **Markdown format** with GitHub-flavored extensions
- **Clear headings** and table of contents
- **Code examples** where applicable
- **Cross-references** between related documents
- **Quick reference sections** for experienced users
- **Read time estimates** for each document

## 🔄 Document Maintenance

### When to Update

- **TESTING_GUIDE.md**: When testing practices change
- **TEST_ORGANIZATION.md**: When test structure changes
- **ENTITY_DATA_FORMAT_*.md**: When implementing entity system or changing conversion approach

### How to Update

1. Make changes to the relevant document
2. Update cross-references if needed
3. Update this README if document relationships change
4. Update read time estimates if document size changes significantly

## 🤝 Contributing to Documentation

When adding new documentation:

1. **Create the document** in this directory
2. **Add entry** to this README with:
   - Document title and filename
   - Brief description
   - Estimated read time
   - Purpose and best use case
3. **Update navigation** section if needed
4. **Update relationships diagram** if document relates to others
5. **Follow standards** listed above

## 📞 Questions?

If you can't find what you're looking for:

1. **Check the main README** at project root
2. **Review existing documents** - use Ctrl+F to search
3. **Check inline code documentation** (Doxygen comments)
4. **Open an issue** on GitHub for clarification

---

**Last Updated**: 2026-01-30  
**Document Count**: 6 (5 technical + this README)  
**Total Size**: ~90KB of documentation  
**Status**: ✅ Up to date
