#include <iostream>
#include <stack>
#include <string>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <vector>  // 新增：包含vector头文件，解决编译报错

using namespace std;

class RPNCalculator {
private:
    stack<double> numStack;  // 存储数字的栈
    string lastError;        // 记录最后一次错误信息

public:
    // 构造函数
    RPNCalculator() : lastError("") {}

    // 向栈中压入数字
    void push(double value) {
        numStack.push(value);
        lastError = "";
    }

    // 从栈中弹出数字（栈空时抛出异常）
    double pop() {
        if (numStack.empty()) {
            lastError = "错误：栈为空，无法弹出数字";
            throw runtime_error(lastError);
        }
        double topVal = numStack.top();
        numStack.pop();
        lastError = "";
        return topVal;
    }

    // 执行单个运算操作
    void calculate(const string& operation) {
        try {
            if (operation.size() != 1) {
                lastError = "错误：不支持多字符操作符";
                throw invalid_argument(lastError);
            }

            char op = operation[0];
            double b = pop();  // 逆波兰表示法：先弹出右操作数
            double a = pop();
            double result = 0.0;

            switch (op) {
                case '+':
                    result = a + b;
                    break;
                case '-':
                    result = a - b;
                    break;
                case '*':
                    result = a * b;
                    break;
                case '/':
                    if (b == 0) {
                        lastError = "错误：除数不能为0";
                        throw runtime_error(lastError);
                    }
                    result = a / b;
                    break;
                case '^':  // 高级功能：幂运算（a的b次方）
                    result = pow(a, b);
                    break;
                case 's':  // 高级功能：平方根（对栈顶元素操作）
                    push(a); // 重新压入前一个操作数，仅计算栈顶的平方根
                    if (b < 0) {
                        lastError = "错误：负数无法计算平方根";
                        throw runtime_error(lastError);
                    }
                    result = sqrt(b);
                    break;
                default:
                    lastError = "错误：不支持的操作符 '" + operation + "'";
                    throw invalid_argument(lastError);
            }

            push(result);  // 将运算结果压入栈中
            lastError = "";
        } catch (const exception& e) {
            throw; // 向上抛出异常，由上层处理
        }
    }

    // 批量处理RPN表达式（空格分隔的数字和操作符）
    double processExpression(const string& expression) {
        lastError = "";
        istringstream iss(expression);
        string token;

        try {
            while (iss >> token) {
                // 判断是否为数字（支持整数、浮点数、负数）
                bool isNumber = true;
                size_t dotCount = 0;
                size_t startIdx = 0;

                // 处理负号（仅允许开头有一个负号）
                if (token[0] == '-' && token.size() > 1) {
                    startIdx = 1;
                }

                // 遍历字符判断是否为合法数字
                for (size_t i = startIdx; i < token.size(); ++i) {
                    if (token[i] == '.') {
                        dotCount++;
                        if (dotCount > 1) { // 最多一个小数点
                            isNumber = false;
                            break;
                        }
                    } else if (!isdigit(token[i])) { // 非数字字符
                        isNumber = false;
                        break;
                    }
                }

                if (isNumber) {
                    // 合法数字，转换后压入栈
                    double num = stod(token);
                    push(num);
                } else {
                    // 操作符，执行运算
                    calculate(token);
                }
            }

            // 表达式合法时，栈中应只剩一个结果
            if (numStack.size() != 1) {
                lastError = "错误：表达式格式无效，栈中剩余 " + to_string(numStack.size()) + " 个数字";
                throw runtime_error(lastError);
            }

            return numStack.top();
        } catch (const exception& e) {
            clearStack(); // 异常时清空栈，避免影响后续操作
            throw;
        }
    }

    // 清空栈
    void clearStack() {
        while (!numStack.empty()) {
            numStack.pop();
        }
        lastError = "";
    }

    // 显示当前栈内容（从栈底到栈顶）
    void displayStack() const {
        stack<double> tempStack = numStack; // 复制栈用于遍历
        cout << "当前栈内容（从栈底到栈顶）：";
        if (tempStack.empty()) {
            cout << "空" << endl;
            return;
        }

        // 反向存储栈元素，实现栈底到栈顶输出
        vector<double> elements;
        while (!tempStack.empty()) {
            elements.push_back(tempStack.top());
            tempStack.pop();
        }

        for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
            cout << *it << " ";
        }
        cout << endl;
    }

    // 获取最后一次错误信息
    string getLastError() const {
        return lastError;
    }

    // 高级功能：斐波那契数列计算（第n项）
    double fibonacci() {
        try {
            int n = static_cast<int>(pop());
            if (n < 0) {
                lastError = "错误：斐波那契数列项数不能为负数";
                throw invalid_argument(lastError);
            }
            // 斐波那契数列定义：F(0)=0, F(1)=1, F(n)=F(n-1)+F(n-2)
            if (n == 0) {
                push(0);
                return 0;
            }
            if (n == 1 || n == 2) {
                push(1);
                return 1;
            }

            double a = 1, b = 1;
            for (int i = 3; i <= n; ++i) {
                double c = a + b;
                a = b;
                b = c;
            }
            push(b);
            return b;
        } catch (const exception& e) {
            throw;
        }
    }
};

// 交互式命令行界面
void runInteractiveMode() {
    RPNCalculator calc;
    string input;

    // 欢迎信息与使用说明
    cout << "==================== C++ RPN 计算器 ====================" << endl;
    cout << "使用说明：" << endl;
    cout << "1. 输入逆波兰表达式（空格分隔数字和操作符，例：5 5 +）" << endl;
    cout << "2. 支持算术操作符：+（加）、-（减）、*（乘）、/（除）" << endl;
    cout << "3. 支持高级操作符：^（幂运算）、s（平方根）" << endl;
    cout << "4. 内置命令：clear（清空栈）、stack（显示栈）、f（斐波那契）、q（退出）" << endl;
    cout << "========================================================" << endl;
    cout << "请输入表达式或命令：" << endl;

    while (true) {
        cout << "> ";
        getline(cin, input);

        // 处理退出命令
        if (input == "q" || input == "Q") {
            cout << "计算器已退出，感谢使用！" << endl;
            break;
        }

        // 处理清空栈命令
        if (input == "clear") {
            calc.clearStack();
            cout << "提示：栈已成功清空" << endl;
            continue;
        }

        // 处理显示栈命令
        if (input == "stack") {
            calc.displayStack();
            continue;
        }

        // 处理斐波那契命令
        if (input == "f") {
            try {
                double fibResult = calc.fibonacci();
                cout << "斐波那契计算结果：" << fibResult << endl;
            } catch (const exception& e) {
                cout << e.what() << endl;
            }
            continue;
        }

        // 处理空输入
        if (input.empty()) {
            continue;
        }

        // 处理普通RPN表达式
        try {
            double result = calc.processExpression(input);
            cout << "计算结果：" << result << endl;
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
}

// 主函数
int main() {
    try {
        runInteractiveMode();
    } catch (const exception& e) {
        cout << "程序异常退出：" << e.what() << endl;
        return 1;
    }
    return 0;
}

