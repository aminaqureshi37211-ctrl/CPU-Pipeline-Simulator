#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

using namespace std;

// Color constants
#define COLOR_BACKGROUND RGB(240, 245, 255)     // Light blue background
#define COLOR_TEXT_AREA RGB(255, 255, 240)      // Ivory text area
#define COLOR_TITLE RGB(0, 0, 100)              // Dark blue title
#define COLOR_BUTTON RGB(70, 130, 180)          // Steel blue buttons
#define COLOR_LIST_BG RGB(255, 250, 240)        // Light yellow for lists

// Instruction structure
struct Instruction {
    string type;
    int stages;
    int duration; // in cycles per stage
};

// CPU Pipeline Simulator Class
class PipelineSimulator {
private:
    vector<string> instructions;
    vector<Instruction> instrList;
    int pipelineStages;

public:
    PipelineSimulator() : pipelineStages(5) {}

    void addInstruction(const string& instr) {
        instructions.push_back(instr);
        Instruction i;
        i.type = instr;

        // Assign stages based on instruction type
        if (instr == "ADD" || instr == "SUB") {
            i.stages = 5;
            i.duration = 1;
        }
        else if (instr == "LOAD" || instr == "STORE") {
            i.stages = 5;
            i.duration = 2; // Memory operations take longer
        }
        else if (instr == "MUL") {
            i.stages = 5;
            i.duration = 3; // Multiplication takes longer
        }
        else if (instr == "JMP") {
            i.stages = 3;   // Branch instructions might have fewer stages
            i.duration = 1;
        }
        else {
            i.stages = 5;
            i.duration = 1;
        }

        instrList.push_back(i);
    }

    void clearInstructions() {
        instructions.clear();
        instrList.clear();
    }

    int getNonPipelinedCycles() {
        int total = 0;
        for (const auto& instr : instrList) {
            total += instr.stages * instr.duration;
        }
        return total;
    }

    int getPipelinedCycles() {
        if (instrList.empty()) return 0;

        int total = pipelineStages; // Fill pipeline
        for (size_t i = 1; i < instrList.size(); i++) {
            total += max(instrList[i-1].duration, instrList[i].duration);
        }
        return total;
    }

    float getSpeedup() {
        int nonPipe = getNonPipelinedCycles();
        int pipe = getPipelinedCycles();
        if (pipe == 0) return 0.0f;
        return (float)nonPipe / pipe;
    }

    vector<string> getInstructions() const { return instructions; }
    int getInstructionCount() const { return instructions.size(); }
};

// Global simulator instance
PipelineSimulator simulator;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    // Register the window class
    const char CLASS_NAME[] = "PipelineSimulator";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(COLOR_BACKGROUND);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    // Register the window class
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Create the window
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles
        CLASS_NAME,                     // Window class
        "CPU Pipeline Simulator",       // Window title
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "Window Creation Failed!", "Error",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Run the message loop
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hOutput;
    static HWND hInstructionInput;
    static HWND hInstrList;
    static HBRUSH hTextBrush = CreateSolidBrush(COLOR_TEXT_AREA);
    static HBRUSH hListBrush = CreateSolidBrush(COLOR_LIST_BG);
    static HBRUSH hTitleBrush = CreateSolidBrush(RGB(220, 230, 255));
    static HBRUSH hButtonBrush = CreateSolidBrush(COLOR_BUTTON);

    switch (msg) {
        case WM_CREATE: {
            // Title
            CreateWindow("STATIC",
                "CPU PIPELINE SIMULATOR",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                10, 10, 780, 25, hwnd, (HMENU)100, NULL, NULL);

            // Instruction input label
            CreateWindow("STATIC", "Enter Instruction:",
                WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE,
                20, 45, 120, 20, hwnd, (HMENU)101, NULL, NULL);

            // Instruction input box
            hInstructionInput = CreateWindow("EDIT", "",
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,
                150, 45, 100, 20, hwnd, (HMENU)102, NULL, NULL);

            // Buttons
            CreateWindow("BUTTON", "Add",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                260, 45, 60, 20, hwnd, (HMENU)1, NULL, NULL);

            CreateWindow("BUTTON", "Clear All",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                330, 45, 80, 20, hwnd, (HMENU)2, NULL, NULL);

            // Simulation buttons
            CreateWindow("BUTTON", "Non-Pipeline",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                20, 80, 100, 25, hwnd, (HMENU)3, NULL, NULL);

            CreateWindow("BUTTON", "Pipeline",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                130, 80, 100, 25, hwnd, (HMENU)4, NULL, NULL);

            CreateWindow("BUTTON", "Compare",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                240, 80, 100, 25, hwnd, (HMENU)5, NULL, NULL);

            CreateWindow("BUTTON", "Stages Info",
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_TABSTOP,
                350, 80, 100, 25, hwnd, (HMENU)6, NULL, NULL);

            // Instruction list label
            CreateWindow("STATIC", "Instruction List:",
                WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE,
                20, 120, 100, 20, hwnd, (HMENU)103, NULL, NULL);

            // Instruction list display
            hInstrList = CreateWindow("EDIT", "",
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL |
                ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                20, 145, 200, 150, hwnd, (HMENU)104, NULL, NULL);

            // Output text area
            hOutput = CreateWindow("EDIT", "",
                WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL |
                ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
                240, 145, 520, 350, hwnd, (HMENU)105, NULL, NULL);

            // Set font for output
            HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                    ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                                    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                    FIXED_PITCH | FF_DONTCARE, "Courier New");
            if (hFont) {
                SendMessage(hOutput, WM_SETFONT, (WPARAM)hFont, TRUE);
                SendMessage(hInstrList, WM_SETFONT, (WPARAM)hFont, TRUE);
                SendMessage(hInstructionInput, WM_SETFONT, (WPARAM)hFont, TRUE);
            }

            // Initial instructions in output
            string initialText =
                "CPU PIPELINE SIMULATOR\n"
                "======================\n\n"
                "Supported Instructions:\n"
                "• ADD  - Add operation\n"
                "• SUB  - Subtract operation\n"
                "• LOAD - Load from memory\n"
                "• STORE- Store to memory\n"
                "• MUL  - Multiply operation\n"
                "• JMP  - Jump instruction\n\n"
                "How to use:\n"
                "1. Type an instruction above and click 'Add'\n"
                "2. Build your instruction sequence\n"
                "3. Click simulation buttons to see results";

            SetWindowText(hOutput, initialText.c_str());
            break;
        }

        case WM_CTLCOLOREDIT: {
            HDC hdc = (HDC)wParam;
            HWND hwndEdit = (HWND)lParam;
            LONG_PTR id = GetWindowLongPtr(hwndEdit, GWLP_ID);

            if (id == 105) { // Output text area
                SetTextColor(hdc, RGB(0, 0, 0)); // Black text
                SetBkColor(hdc, COLOR_TEXT_AREA); // Ivory background
                return (LRESULT)hTextBrush;
            }
            else if (id == 104) { // Instruction list
                SetTextColor(hdc, RGB(0, 0, 0)); // Black text
                SetBkColor(hdc, COLOR_LIST_BG); // Light yellow background
                return (LRESULT)hListBrush;
            }
            else if (id == 102) { // Input edit
                SetTextColor(hdc, RGB(0, 0, 0)); // Black text
                SetBkColor(hdc, RGB(255, 255, 255)); // White background
                return (LRESULT)GetStockObject(WHITE_BRUSH);
            }
            break;
        }

        case WM_CTLCOLORSTATIC: {
            HDC hdc = (HDC)wParam;
            HWND hwndStatic = (HWND)lParam;
            LONG_PTR id = GetWindowLongPtr(hwndStatic, GWLP_ID);

            if (id == 100) { // Title
                SetTextColor(hdc, COLOR_TITLE); // Dark blue text
                SetBkColor(hdc, RGB(220, 230, 255)); // Light blue background
                return (LRESULT)hTitleBrush;
            }
            else if (id == 101 || id == 103) { // Labels
                SetTextColor(hdc, RGB(0, 0, 128)); // Navy blue text
                SetBkColor(hdc, COLOR_BACKGROUND); // Match window background
                return (LRESULT)CreateSolidBrush(COLOR_BACKGROUND);
            }
            break;
        }

        case WM_CTLCOLORBTN: {
            HDC hdc = (HDC)wParam;
            SetTextColor(hdc, RGB(255, 255, 255)); // White text
            SetBkColor(hdc, COLOR_BUTTON); // Steel blue background
            return (LRESULT)hButtonBrush;
        }

        case WM_COMMAND: {
            int wmId = LOWORD(wParam);

            switch (wmId) {
                case 1: { // Add Instruction
                    char instrText[32];
                    GetWindowText(hInstructionInput, instrText, sizeof(instrText));
                    string instruction(instrText);

                    // Trim whitespace and convert to uppercase
                    instruction.erase(0, instruction.find_first_not_of(" \t\n\r"));
                    instruction.erase(instruction.find_last_not_of(" \t\n\r") + 1);

                    if (instruction.empty()) {
                        MessageBox(hwnd, "Please enter an instruction",
                                 "Input Error", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    // Convert to uppercase
                    transform(instruction.begin(), instruction.end(),
                             instruction.begin(), ::toupper);

                    // Validate instruction
                    vector<string> validInstr = {"ADD", "SUB", "LOAD", "STORE", "MUL", "JMP"};
                    bool isValid = false;
                    for (const auto& valid : validInstr) {
                        if (instruction == valid) {
                            isValid = true;
                            break;
                        }
                    }

                    if (!isValid) {
                        string errorMsg = "Invalid instruction!\n\nValid instructions are:\n";
                        for (const auto& valid : validInstr) {
                            errorMsg += valid + "\n";
                        }
                        MessageBox(hwnd, errorMsg.c_str(),
                                 "Error", MB_OK | MB_ICONERROR);
                        break;
                    }

                    // Add to simulator
                    simulator.addInstruction(instruction);

                    // Update instruction list display
                    stringstream instrList;
                    vector<string> instructions = simulator.getInstructions();
                    for (size_t i = 0; i < instructions.size(); i++) {
                        instrList << i+1 << ". " << instructions[i] << "\n";
                    }
                    SetWindowText(hInstrList, instrList.str().c_str());

                    // Clear input field and set focus
                    SetWindowText(hInstructionInput, "");
                    SetFocus(hInstructionInput);

                    // Show confirmation
                    stringstream output;
                    output << "Instruction added: " << instruction << "\n";
                    output << "Total instructions: " << simulator.getInstructionCount() << "\n\n";
                    output << "Click simulation buttons to analyze performance.";
                    SetWindowText(hOutput, output.str().c_str());
                    break;
                }

                case 2: { // Clear All
                    if (simulator.getInstructionCount() > 0) {
                        int result = MessageBox(hwnd,
                            "Clear all instructions?",
                            "Confirm Clear",
                            MB_YESNO | MB_ICONQUESTION);

                        if (result == IDYES) {
                            simulator.clearInstructions();
                            SetWindowText(hInstrList, "");
                            SetWindowText(hOutput, "All instructions cleared.\nReady for new sequence.");
                        }
                    }
                    break;
                }

                case 3: { // Non-Pipeline Simulation
                    if (simulator.getInstructionCount() == 0) {
                        MessageBox(hwnd, "No instructions to simulate!",
                                 "Warning", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    stringstream output;
                    output << "NON-PIPELINE SIMULATION\n";
                    output << string(25, '=') << "\n\n";

                    output << "Instruction Sequence:\n";
                    vector<string> instructions = simulator.getInstructions();
                    for (size_t i = 0; i < instructions.size(); i++) {
                        output << "  " << i+1 << ". " << instructions[i] << "\n";
                    }
                    output << "\n";

                    output << "Execution Pattern:\n";
                    output << "• Each instruction completes all stages before next begins\n";
                    output << "• No overlapping of instructions\n";
                    output << "• Simple but inefficient\n\n";

                    int totalCycles = simulator.getNonPipelinedCycles();
                    int instrCount = simulator.getInstructionCount();

                    output << "Performance Analysis:\n";
                    output << "• Total Cycles: " << totalCycles << "\n";
                    output << "• Instructions: " << instrCount << "\n";
                    output << "• Throughput: 1 instruction per "
                           << (totalCycles/instrCount) << " cycles\n";
                    output << "• CPI (Cycles Per Instruction): "
                           << fixed << setprecision(2)
                           << ((float)totalCycles/instrCount) << "\n\n";

                    output << "Cycle Usage:\n";
                    for (int i = 0; i < instrCount; i++) {
                        output << "  I" << i+1 << ": Cycles "
                               << (i*5+1) << "-" << (i*5+5) << "\n";
                    }

                    SetWindowText(hOutput, output.str().c_str());
                    break;
                }

                case 4: { // Pipeline Simulation
                    if (simulator.getInstructionCount() == 0) {
                        MessageBox(hwnd, "No instructions to simulate!",
                                 "Warning", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    stringstream output;
                    output << "PIPELINE SIMULATION\n";
                    output << string(20, '=') << "\n\n";

                    output << "Instruction Sequence:\n";
                    vector<string> instructions = simulator.getInstructions();
                    for (size_t i = 0; i < instructions.size(); i++) {
                        output << "  " << i+1 << ". " << instructions[i] << "\n";
                    }
                    output << "\n";

                    output << "5-Stage Pipeline:\n";
                    output << "IF  - Instruction Fetch\n";
                    output << "ID  - Instruction Decode\n";
                    output << "EX  - Execute\n";
                    output << "MEM - Memory Access\n";
                    output << "WB  - Write Back\n\n";

                    int pipeCycles = simulator.getPipelinedCycles();
                    int instrCount = simulator.getInstructionCount();

                    output << "Pipeline Timeline (First 20 cycles):\n";
                    output << "Cycle\tActive Instructions\n";
                    output << string(40, '-') << "\n";

                    for (int cycle = 1; cycle <= min(20, pipeCycles); cycle++) {
                        output << setw(3) << cycle << "\t";
                        for (int i = 0; i < instrCount; i++) {
                            if (cycle >= i+1 && cycle <= i+5) {
                                output << "I" << i+1 << " ";
                            }
                        }
                        output << "\n";
                    }

                    if (pipeCycles > 20) {
                        output << "... continues to cycle " << pipeCycles << "\n";
                    }
                    output << "\n";

                    output << "Performance Analysis:\n";
                    output << "• Total Cycles: " << pipeCycles << "\n";
                    output << "• Instructions: " << instrCount << "\n";
                    output << "• Pipeline Fill: 5 cycles\n";
                    output << "• Pipeline Drain: 4 cycles\n";
                    output << "• Steady State: 1 instruction/cycle\n";
                    output << "• CPI: " << fixed << setprecision(2)
                           << ((float)pipeCycles/instrCount) << "\n";

                    SetWindowText(hOutput, output.str().c_str());
                    break;
                }

                case 5: { // Compare Performance
                    if (simulator.getInstructionCount() == 0) {
                        MessageBox(hwnd, "No instructions to compare!",
                                 "Warning", MB_OK | MB_ICONWARNING);
                        break;
                    }

                    stringstream output;
                    output << "PERFORMANCE COMPARISON\n";
                    output << string(25, '=') << "\n\n";

                    int nonPipe = simulator.getNonPipelinedCycles();
                    int pipe = simulator.getPipelinedCycles();
                    float speedup = simulator.getSpeedup();

                    output << "Configuration:\n";
                    output << "• Instructions: " << simulator.getInstructionCount() << "\n";
                    output << "• Pipeline Stages: 5\n\n";

                    output << "Results:\n";
                    output << "• Non-Pipeline Cycles: " << nonPipe << "\n";
                    output << "• Pipeline Cycles: " << pipe << "\n";
                    output << "• Speedup: " << fixed << setprecision(2)
                           << speedup << "x faster\n";
                    output << "• Cycles Saved: " << (nonPipe - pipe) << "\n";
                    output << "• Efficiency Gain: " << setprecision(1)
                           << ((float)(nonPipe - pipe) * 100 / nonPipe) << "%\n\n";

                    // Visual comparison
                    output << "Visual Comparison:\n";
                    output << "Non-Pipeline: ";
                    int nonPipeBars = min(30, nonPipe / 2);
                    for (int i = 0; i < nonPipeBars; i++) output << "█";
                    output << " [" << nonPipe << " cycles]\n";

                    output << "Pipeline:     ";
                    int pipeBars = min(30, pipe / 2);
                    for (int i = 0; i < pipeBars; i++) output << "█";
                    output << " [" << pipe << " cycles]\n\n";

                    output << "Analysis:\n";
                    if (speedup > 3.0) {
                        output << "✓ Excellent pipeline utilization\n";
                        output << "✓ Large instruction count benefits pipeline\n";
                    } else if (speedup > 1.8) {
                        output << "✓ Good pipeline efficiency\n";
                        output << "✓ Clear advantage over non-pipelined\n";
                    } else if (speedup > 1.2) {
                        output << "✓ Moderate improvement\n";
                        output << "✓ Pipeline shows benefits\n";
                    } else {
                        output << "⚠ Small improvement due to few instructions\n";
                        output << "⚠ Add more instructions for better pipeline utilization\n";
                    }

                    SetWindowText(hOutput, output.str().c_str());
                    break;
                }

                case 6: { // Stages Info
                    stringstream output;
                    output << "PIPELINE STAGES INFORMATION\n";
                    output << string(30, '=') << "\n\n";

                    output << "5-Stage RISC Pipeline Architecture:\n\n";

                    output << "1. IF - INSTRUCTION FETCH\n";
                    output << "   • Fetch next instruction from memory\n";
                    output << "   • Increment Program Counter (PC)\n";
                    output << "   • Duration: 1 cycle\n\n";

                    output << "2. ID - INSTRUCTION DECODE\n";
                    output << "   • Decode opcode and operands\n";
                    output << "   • Read register values\n";
                    output << "   • Hazard detection unit\n";
                    output << "   • Duration: 1 cycle\n\n";

                    output << "3. EX - EXECUTE\n";
                    output << "   • Perform ALU operation\n";
                    output << "   • Calculate memory address\n";
                    output << "   • Duration: 1-3 cycles (instruction dependent)\n";
                    output << "     - ADD/SUB: 1 cycle\n";
                    output << "     - LOAD/STORE: 2 cycles\n";
                    output << "     - MUL: 3 cycles\n\n";

                    output << "4. MEM - MEMORY ACCESS\n";
                    output << "   • Load data from memory\n";
                    output << "   • Store data to memory\n";
                    output << "   • Duration: 1-2 cycles\n\n";

                    output << "5. WB - WRITE BACK\n";
                    output << "   • Write result to register file\n";
                    output << "   • Duration: 1 cycle\n\n";

                    output << "PIPELINE HAZARDS:\n";
                    output << "1. Data Hazards - Instruction depends on result of previous\n";
                    output << "2. Control Hazards - Branch instructions change program flow\n";
                    output << "3. Structural Hazards - Multiple instructions need same resource\n\n";

                    output << "PERFORMANCE METRICS:\n";
                    output << "• Speedup = Non-Pipeline Cycles / Pipeline Cycles\n";
                    output << "• Throughput = Instructions / Total Cycles\n";
                    output << "• CPI = Cycles Per Instruction\n";

                    SetWindowText(hOutput, output.str().c_str());
                    break;
                }
            }
            break;
        }

        case WM_DESTROY:
            if (hTextBrush) DeleteObject(hTextBrush);
            if (hListBrush) DeleteObject(hListBrush);
            if (hTitleBrush) DeleteObject(hTitleBrush);
            if (hButtonBrush) DeleteObject(hButtonBrush);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
