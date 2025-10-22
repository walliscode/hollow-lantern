# 🎨 Visual Architecture Diagrams

## Current Architecture (Qt6)

### Component Diagram
```
┌────────────────────────────────────────────────────────────────────┐
│                         HOLLOW LANTERN                             │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  ┌─────────────────────┐         ┌─────────────────────┐          │
│  │  hollow-lantern     │         │ hollow-lantern-gui  │          │
│  │  (CLI - SFML)       │         │ (GUI - Qt6)         │          │
│  │                     │         │                     │          │
│  │  ┌───────────────┐  │         │  ┌───────────────┐  │          │
│  │  │ SFML Window   │  │         │  │ QApplication  │  │          │
│  │  │               │  │         │  │               │  │          │
│  │  │ Renders       │  │         │  │ ┌──────────┐  │  │          │
│  │  │ Projections   │  │         │  │ │QMainWindow│ │  │          │
│  │  │               │  │         │  │ │          │  │  │          │
│  │  │ (Hardcoded    │  │         │  │ │ QMenuBar │  │  │          │
│  │  │  model)       │  │         │  │ │ QStatusBar│ │  │          │
│  │  └───────────────┘  │         │  │ └──────────┘  │  │          │
│  └─────────────────────┘         │  └───────────────┘  │          │
│           │                      │           │         │          │
│           │                      │           │         │          │
│           └──────────┬───────────┴───────────┘         │          │
│                      │                                 │          │
│              ┌───────▼──────────┐                      │          │
│              │  Core Libraries  │                      │          │
│              │                  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │ VoxReader  │  │                      │          │
│              │  └────────────┘  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │VoxManipula.│  │                      │          │
│              │  └────────────┘  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │ Projector  │  │                      │          │
│              │  └────────────┘  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │ ModelData  │  │                      │          │
│              │  └────────────┘  │                      │          │
│              └──────────────────┘                      │          │
│                                                        │          │
└────────────────────────────────────────────────────────────────────┘

Dependencies:
- SFML 3.0 (for CLI app)
- Qt6 Core, Widgets, OpenGLWidgets (for GUI app)
- GLM, nlohmann/json, CGAL, Catch2

Issues:
❌ Two separate graphics systems
❌ Qt not integrated with SFML rendering
❌ No runtime parameter adjustment
❌ Heavy dependencies for simple needs
```

---

## Proposed Architecture (ImGui-SFML)

### Component Diagram
```
┌────────────────────────────────────────────────────────────────────┐
│                         HOLLOW LANTERN                             │
├────────────────────────────────────────────────────────────────────┤
│                                                                    │
│  ┌─────────────────────┐         ┌─────────────────────┐          │
│  │  hollow-lantern     │         │ hollow-lantern-gui  │          │
│  │  (CLI - SFML)       │         │ (GUI - SFML+ImGui)  │          │
│  │  [Can be removed]   │         │                     │          │
│  │                     │         │  ┌───────────────┐  │          │
│  │  ┌───────────────┐  │         │  │ SFML Window   │  │          │
│  │  │ SFML Window   │  │         │  │               │  │          │
│  │  │               │  │         │  │ ┌───────────┐ │  │          │
│  │  │ Simple        │  │         │  │ │Model      │ │  │          │
│  │  │ Animation     │  │         │  │ │Projection │ │  │          │
│  │  └───────────────┘  │         │  │ │(SFML)     │ │  │          │
│  └─────────────────────┘         │  │ └───────────┘ │  │          │
│           │                      │  │               │  │          │
│           │                      │  │ ┌───────────┐ │  │          │
│           │                      │  │ │ImGui UI   │ │  │          │
│           │                      │  │ │Overlay    │ │  │          │
│           │                      │  │ │           │ │  │          │
│           │                      │  │ │┌─────────┐│ │  │          │
│           │                      │  │ ││Menu Bar ││ │  │          │
│           │                      │  │ │└─────────┘│ │  │          │
│           │                      │  │ │┌─────┬───┐│ │  │          │
│           │                      │  │ ││Info │Ctl││ │  │          │
│           │                      │  │ │└─────┴───┘│ │  │          │
│           │                      │  │ └───────────┘ │  │          │
│           │                      │  └───────────────┘  │          │
│           │                      │           │         │          │
│           └──────────┬───────────┴───────────┘         │          │
│                      │                                 │          │
│              ┌───────▼──────────┐                      │          │
│              │  Core Libraries  │                      │          │
│              │  (Unchanged)     │                      │          │
│              │                  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │ VoxReader  │  │                      │          │
│              │  └────────────┘  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │VoxManipula.│  │                      │          │
│              │  └────────────┘  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │ Projector  │  │                      │          │
│              │  └────────────┘  │                      │          │
│              │  ┌────────────┐  │                      │          │
│              │  │ ModelData  │  │                      │          │
│              │  └────────────┘  │                      │          │
│              └──────────────────┘                      │          │
│                                                        │          │
└────────────────────────────────────────────────────────────────────┘

Dependencies:
- SFML 3.0 (unified graphics)
- ImGui + ImGui-SFML (UI overlay)
- GLM, nlohmann/json, CGAL, Catch2

Benefits:
✅ Single graphics system (SFML)
✅ ImGui naturally overlays SFML
✅ Perfect for runtime parameters
✅ Lighter dependencies
✅ Simpler architecture
```

---

## UI Layout Comparison

### Current (Qt6)

```
╔════════════════════════════════════════════════════════════════╗
║ Hollow Lantern                                          [_][□][×]║
╠════════════════════════════════════════════════════════════════╣
║ File   Edit   View   Help                                     ║
╠════════════════════════════════════════════════════════════════╣
║                                                                ║
║                                                                ║
║                     [EMPTY CENTRAL WIDGET]                     ║
║                                                                ║
║                    (Planned: Model viewer)                     ║
║                                                                ║
║                                                                ║
╠════════════════════════════════════════════════════════════════╣
║ Ready                                                          ║
╚════════════════════════════════════════════════════════════════╝

Features:
- QMenuBar with File menu (Open, Exit)
- QStatusBar with status message
- Empty central widget
- Signal/slot connections for actions
```

### Proposed (ImGui-SFML)

```
╔════════════════════════════════════════════════════════════════╗
║ Hollow Lantern                                          [_][□][×]║
╠════════════════════════════════════════════════════════════════╣
║ [File ▼] [View ▼] [Help ▼]                                    ║
╠═══════════════════════════════╦════════════════════════════════╣
║                               ║ ┌─ Model Information ───┐     ║
║                               ║ │ Name: colour_cube.vox │     ║
║                               ║ │ Size: 16×16×16        │     ║
║        MODEL PROJECTION       ║ │ Voxels: 896           │     ║
║     (SFML rendered voxels)    ║ │ Triangles: 1234       │     ║
║                               ║ │ Memory: ~2.5 MB       │     ║
║                               ║ └───────────────────────┘     ║
║                               ║                               ║
║                               ║ ┌─ Controls ────────────┐     ║
║                               ║ │ Rotation: [=====    ] │     ║
║                               ║ │     X: [====        ] │     ║
║                               ║ │     Y: [======      ] │     ║
║                               ║ │     Z: [===         ] │     ║
║                               ║ │                       │     ║
║                               ║ │ Zoom: [=======      ] │     ║
║                               ║ │                       │     ║
║                               ║ │ [Apply] [Reset]       │     ║
║                               ║ └───────────────────────┘     ║
╠═══════════════════════════════╩════════════════════════════════╣
║ Loaded: colour_cube.vox                         FPS: 60        ║
╚════════════════════════════════════════════════════════════════╝

Features:
- ImGui menu bar (lightweight)
- SFML projection in main area
- ImGui windows floating over rendering
- Real-time slider controls
- Direct value updates
- FPS counter
```

---

## Data Flow Diagrams

### Current (Qt6)

```
User Action (Click "Open")
         │
         ▼
    QAction::triggered() signal
         │
         ▼
    MainWindow::OnOpenFile() slot
         │
         ▼
    QFileDialog::getOpenFileName()
         │
         ▼
    emit FileOpenRequested(path) signal
         │
         ▼
    [Not Connected Yet - Planned]
         │
         ▼
    VoxReader::ProvideVoxData()
         │
         ▼
    ModelData loaded
         │
         ▼
    [Display in widget - Planned]


Issues:
❌ Complex signal/slot chain
❌ Requires connecting everything
❌ State stored in widgets
❌ Not integrated with SFML
```

### Proposed (ImGui-SFML)

```
User Action (Click "Open" in menu)
         │
         ▼
    ImGui::MenuItem() returns true
         │
         ▼
    ShowFileDialog() function call
         │
         ▼
    User selects file
         │
         ▼
    LoadModelFile(path) function call
         │
         ▼
    VoxReader::ProvideVoxData(path)
         │
         ▼
    ModelData loaded → stored in current_model_
         │
         ▼
    Next frame: RenderModelView()
         │
         ▼
    SFML draws projection
         │
         ▼
    RenderModelInfoWindow()
         │
         ▼
    ImGui displays stats from current_model_


Benefits:
✅ Direct function calls
✅ Simple control flow
✅ State in normal variables
✅ Integrated with SFML
✅ Real-time updates
```

---

## File Structure Changes

### Before (Current)

```
hollow-lantern/
├── CMakeLists.txt                     [Uses Qt6]
├── cmake/
│   └── fetch-content-modules/
│       ├── SFML.cmake                 [SFML for CLI]
│       └── ... (other modules)
├── src/
│   ├── gui/                           [Qt6 based]
│   │   ├── MainWindow.h               ❌ Remove
│   │   ├── MainWindow.cpp             ❌ Remove
│   │   └── CMakeLists.txt             ❌ Remove (Qt MOC/UIC)
│   ├── readers/                       ✅ Keep
│   ├── manipulators/                  ✅ Keep
│   └── structures/                    ✅ Keep
├── hollow-lantern/                    [CLI - SFML]
│   ├── main.cpp                       ✅ Keep
│   └── CMakeLists.txt                 ✅ Keep
├── hollow-lantern-gui/                [GUI - Qt6]
│   ├── main.cpp                       ❌ Rewrite
│   └── CMakeLists.txt                 ❌ Update (remove Qt)
├── tests/
│   ├── gui/
│   │   ├── MainWindow.test.cpp        ❌ Replace
│   │   └── CMakeLists.txt             ❌ Update (remove Qt Test)
│   └── ... (other tests)              ✅ Keep unchanged
└── docs/
    └── ... (documentation)            📝 Update
```

### After (Proposed)

```
hollow-lantern/
├── CMakeLists.txt                     [No Qt6, has imgui-sfml]
├── cmake/
│   └── fetch-content-modules/
│       ├── SFML.cmake                 ✅ Keep
│       ├── imgui.cmake                ✨ NEW
│       ├── imgui-sfml.cmake           ✨ NEW
│       └── ... (other modules)
├── src/
│   ├── gui/                           [ImGui-SFML based]
│   │   ├── Application.h              ✨ NEW (main app class)
│   │   ├── Application.cpp            ✨ NEW
│   │   ├── GuiState.h                 ✨ NEW (UI state)
│   │   ├── ModelViewer.h              ✨ NEW (SFML rendering)
│   │   ├── ModelViewer.cpp            ✨ NEW
│   │   ├── UiComponents.h             ✨ NEW (ImGui widgets)
│   │   ├── UiComponents.cpp           ✨ NEW
│   │   └── CMakeLists.txt             ✨ NEW (imgui-sfml)
│   ├── readers/                       ✅ Keep
│   ├── manipulators/                  ✅ Keep
│   └── structures/                    ✅ Keep
├── hollow-lantern/                    [CLI - SFML]
│   ├── main.cpp                       ✅ Keep (optional, could remove)
│   └── CMakeLists.txt                 ✅ Keep
├── hollow-lantern-gui/                [GUI - SFML+ImGui]
│   ├── main.cpp                       ✨ REWRITTEN (SFML+ImGui)
│   └── CMakeLists.txt                 ✨ UPDATED (imgui-sfml)
├── tests/
│   ├── gui/
│   │   ├── Application.test.cpp       ✨ NEW (simplified)
│   │   └── CMakeLists.txt             ✨ UPDATED (no Qt Test)
│   └── ... (other tests)              ✅ Keep unchanged
├── docs/
│   ├── qt-reference/                  📦 ARCHIVED Qt code
│   │   ├── src-gui-backup/
│   │   ├── hollow-lantern-gui-backup/
│   │   └── tests-gui-backup/
│   ├── IMGUI_MIGRATION_PLAN.md        ✨ NEW
│   ├── QT_REMOVAL_SUMMARY.md          ✨ NEW
│   ├── QT_VS_IMGUI_COMPARISON.md      ✨ NEW
│   ├── EXECUTIVE_SUMMARY.md           ✨ NEW
│   ├── IMGUI_GUIDELINES.md            ✨ TODO
│   └── ... (other docs)               📝 Update
```

---

## Dependencies Graph

### Current Dependencies

```
hollow-lantern
      │
      ├─── hollow-lantern (CLI)
      │         │
      │         ├─── SFML 3.0 ───────────────┐
      │         ├─── readers                 │
      │         ├─── manipulators            │
      │         └─── structures              │
      │                                      │
      └─── hollow-lantern-gui               │
                │                            │
                ├─── Qt6 Core ───┐           │
                ├─── Qt6 Widgets │           │
                ├─── Qt6 OpenGL ─┘           │
                ├─── gui (Qt based)          │
                │     │                      │
                │     └─── (Needs Qt)        │
                │                            │
                ├─── readers                 │
                ├─── manipulators            │
                └─── structures              │
                                             │
                                             │
      Core Libraries                         │
      ├─── VoxReader ──────────────────────┐ │
      ├─── VoxManipulator                  │ │
      ├─── Projector                       │ │
      └─── ModelData                       │ │
                                           │ │
      External Dependencies                │ │
      ├─── SFML 3.0 ───────────────────────┘ │
      ├─── Qt6 ──────────────────────────────┘
      ├─── GLM
      ├─── nlohmann/json
      ├─── CGAL
      └─── Catch2

Problem: Two graphics systems (SFML + Qt)
```

### Proposed Dependencies

```
hollow-lantern
      │
      ├─── hollow-lantern (CLI - Optional)
      │         │
      │         ├─── SFML 3.0 ───────────────┐
      │         ├─── readers                 │
      │         ├─── manipulators            │
      │         └─── structures              │
      │                                      │
      └─── hollow-lantern-gui               │
                │                            │
                ├─── SFML 3.0 ────────────┐  │
                ├─── ImGui ───────┐       │  │
                ├─── ImGui-SFML ──┼───────┘  │
                │                 │          │
                ├─── gui (ImGui)  │          │
                │     │            │          │
                │     ├─── ImGui ─┘          │
                │     └─── SFML ─────────────┘
                │
                ├─── readers
                ├─── manipulators
                └─── structures

      Core Libraries
      ├─── VoxReader ──────────────────────┐
      ├─── VoxManipulator                  │
      ├─── Projector                       │
      └─── ModelData                       │
                                           │
      External Dependencies                │
      ├─── SFML 3.0 ───────────────────────┘
      ├─── ImGui (lightweight ~500KB)
      ├─── ImGui-SFML (bridge library)
      ├─── GLM
      ├─── nlohmann/json
      ├─── CGAL
      └─── Catch2

Benefit: Single graphics system (SFML)
        ImGui overlays naturally on SFML
        No Qt dependencies
```

---

## Build Process Comparison

### Current (Qt6)

```
cmake --preset Debug
     │
     ├─ Find Qt6 packages ─────────────────┐
     │  (Qt6Core, Qt6Widgets, Qt6OpenGL)   │ [Heavy]
     │                                     │
     ├─ Fetch SFML ────────────────────────┤
     ├─ Fetch GLM, json, etc. ─────────────┤
     │                                     │
     └─ Configure projects                │
                                           │
cmake --build --preset Debug               │
     │                                     │
     ├─ Run Qt MOC (Meta Object Compiler)─┘ [Slow preprocessing]
     │    (Generates moc_*.cpp files)
     │
     ├─ Run Qt UIC (UI Compiler)
     │    (Generates ui_*.h files)
     │
     ├─ Compile core libraries
     │
     ├─ Compile hollow-lantern (CLI)
     │
     ├─ Compile gui library ──────────────┐
     │    (With MOC generated files)      │ [Complex]
     │                                     │
     └─ Link hollow-lantern-gui ──────────┘
          (With Qt libraries)              [Large binary]

Build time: ~2-3 minutes (first time)
Binary size: ~15-20 MB
```

### Proposed (ImGui-SFML)

```
cmake --preset Debug
     │
     ├─ Fetch SFML ────────────────────────┐
     ├─ Fetch ImGui ───────────────────────┤ [All lightweight]
     ├─ Fetch ImGui-SFML ──────────────────┤
     ├─ Fetch GLM, json, etc. ─────────────┤
     │                                     │
     └─ Configure projects                │
          (No MOC/UIC needed)              │
                                           │
cmake --build --preset Debug               │
     │                                     │
     ├─ Compile ImGui ─────────────────────┘ [Fast]
     │    (Just regular C++)
     │
     ├─ Compile ImGui-SFML
     │    (Bridge library)
     │
     ├─ Compile core libraries
     │
     ├─ Compile hollow-lantern (CLI)
     │
     ├─ Compile gui library ──────────────┐
     │    (Pure C++, no preprocessing)   │ [Simple]
     │                                     │
     └─ Link hollow-lantern-gui ──────────┘
          (With SFML + ImGui)              [Smaller binary]

Build time: ~1-2 minutes (first time)
Binary size: ~8-12 MB
```

---

## Testing Strategy

### Current (Qt6 Testing)

```
Test Structure:
├── GUI Tests (Qt Test framework)
│   ├── Widget creation tests
│   ├── Signal/slot tests
│   ├── Menu bar tests
│   ├── Status bar tests
│   ├── Keyboard shortcut tests
│   └── Memory management tests
│
└── Core Tests (Catch2)
    ├── VoxReader tests
    ├── VoxManipulator tests
    └── Projector tests

Running:
$ ctest --preset Debug
  - test_gui (14 tests) ──┐
  - test_vox_reader       │ [Complex GUI tests]
  - test_vox_manipulator  │
  - test_projector        │
  - test_config           │
                          │
All: 14/14 passing ───────┘

Challenges:
- Need QApplication for GUI tests
- Headless testing requires offscreen platform
- Widget state testing is verbose
- Comprehensive but complex
```

### Proposed (ImGui Testing)

```
Test Structure:
├── GUI Tests (Simplified - Catch2)
│   ├── Application creation test
│   ├── Logic tests (file loading)
│   ├── State management tests
│   └── Integration tests
│   (No ImGui rendering tests)
│
└── Core Tests (Catch2) [UNCHANGED]
    ├── VoxReader tests
    ├── VoxManipulator tests
    └── Projector tests

Running:
$ ctest --preset Debug
  - test_gui (simplified) ─┐
  - test_vox_reader        │ [Focus on logic]
  - test_vox_manipulator   │
  - test_projector         │
  - test_config            │
                           │
Fewer tests, but adequate ┘

Plus Manual Testing:
✓ UI appearance
✓ Interaction flow
✓ Real-time controls
✓ Visual feedback

Benefits:
- Simpler test code
- Faster test execution
- Test what matters (logic)
- ImGui itself is well-tested
- Manual testing is quick
```

---

## Timeline Visualization

```
Week 1: Qt to ImGui Migration
┌─────────────────────────────────────────────────────────────────┐
│ Day 1              │ Day 2        │ Day 3-4      │ Day 5-6      │
├─────────────────────────────────────────────────────────────────┤
│ Phase 1:          │ Phase 2:     │ Phase 3-4:   │ Phase 5-7:   │
│ Cleanup           │ Dependencies │ GUI Impl     │ Polish       │
│                   │              │              │              │
│ • Archive Qt      │ • imgui.cmake│ • Application│ • Entry point│
│ • Remove from     │ • imgui-sfml │ • GuiState   │ • Tests      │
│   CMake          │ • Verify      │ • ModelViewer│ • Docs       │
│ • Clean build     │   builds     │ • UiCompon.  │ • Validate   │
│                   │              │ • Features   │              │
│ [========]        │ [==========] │ [==========] │ [=========]  │
│ 0.5 day           │ 1 day        │ 2-3 days     │ 2-3 days     │
└─────────────────────────────────────────────────────────────────┘

Alternative (Stay with Qt):
┌─────────────────────────────────────────────────────────────────┐
│ Week 1-2: Qt Development                                        │
├─────────────────────────────────────────────────────────────────┤
│ • Learn Qt patterns (2-3 days)                                  │
│ • Implement Qt features (4-5 days)                              │
│ • Integrate with SFML (1-2 days)                                │
│ • Testing and polish (1-2 days)                                 │
│                                                                 │
│ [==========================================================]    │
│ 8-12 days                                                       │
└─────────────────────────────────────────────────────────────────┘

Result: ImGui migration is 30-50% faster
```

---

## Summary Comparison Table

| Aspect | Current (Qt6) | Proposed (ImGui-SFML) | Winner |
|--------|---------------|----------------------|--------|
| **Graphics System** | Dual (SFML + Qt) | Single (SFML) | ImGui ✅ |
| **Dependencies** | ~50MB (Qt6) | ~500KB (ImGui) | ImGui ✅ |
| **Build Time** | 2-3 min | 1-2 min | ImGui ✅ |
| **Code Complexity** | High (signals/slots) | Low (direct calls) | ImGui ✅ |
| **Runtime Params** | Moderate fit | Perfect fit | ImGui ✅ |
| **UI Polish** | Professional | Tool-like | Qt ⭕ |
| **File Dialogs** | Native | Need addon | Qt ⭕ |
| **Testing** | Comprehensive | Simplified | Qt ⭕ |
| **Learning Curve** | Moderate | Moderate | Tie ➖ |
| **Development Time** | 8-12 days | 5-7 days | ImGui ✅ |
| **Alignment with Goals** | Poor | Excellent | ImGui ✅ |

**Score: ImGui wins 8 vs Qt wins 3 vs Tie 1**

---

## Next Action

**👉 Awaiting approval to proceed with Phase 1: Qt Removal**

See EXECUTIVE_SUMMARY.md for quick overview or IMGUI_MIGRATION_PLAN.md for detailed implementation plan.

---

**Created:** 2025-10-22  
**Status:** Planning Complete ✅  
**Ready to Proceed:** YES ✅
