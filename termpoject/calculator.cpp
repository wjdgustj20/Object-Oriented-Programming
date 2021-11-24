#include <iostream>
#include <string>
#include <stack>
#include <sstream>
using namespace std;

int throwException(int c) {
	switch (c) {
	case 1: throw string("오른쪽 괄호가 빠졌습니다.");
	case 2: throw string("알 수 없는 연산자입니다.");
	case 3: throw string("잘못된 입력값입니다.");
	case 4: throw string("0으로 나눌 수 없습니다.");
	case 5: throw string("왼쪽 괄호가 빠졌습니다.");
	default:
		return 0;
	}
}

class Strategy {
public:
	virtual int doOperation(int num1, int num2) = 0;
};

class OperationAdd : public Strategy {
public:
	int doOperation(int num1, int num2) {
		return num1 + num2;
	}
};

class OperationSubstract : public Strategy {
public:
	int doOperation(int num1, int num2) {
		return num1 - num2;
	}
};

class OperationMultiply : public Strategy {
public:
	int doOperation(int num1, int num2) {
		return num1 * num2;
	}
};

class OperationDivide : public Strategy {
public:
	int doOperation(int num1, int num2) {
		return num1 / num2;
	}
};

class Context {
	Strategy* strategy;
public:
	Context(Strategy* strategy) {
		this->strategy = strategy;
	}
	int executeStrategy(int num1, int num2) {
		return strategy->doOperation(num1, num2);
	}
};

class PostfixCalculator {
	stack<int> valueStack;
	Context* context;

	void executeBinaryOperator(char anOperator) {
		if (valueStack.size() < 2) {
			try {
				throwException(3);
			}
			catch (string& s) {
				cout << s << endl;
				exit(0);
			}
		}
		int operand1 = valueStack.top();
		valueStack.pop();
		int operand2 = valueStack.top();
		valueStack.pop();
		int calculated = 0;
		switch (anOperator) {
		case '*':
			context = new Context(new OperationMultiply());
			calculated = context->executeStrategy(operand2, operand1);
			break;
		case '/':
			if (operand1 == 0) {
				try {
					throwException(4);
				}
				catch (string& s) {
					cout << s << endl;
					exit(0);
				}
			}
			else {
				context = new Context(new OperationDivide());
				calculated = context->executeStrategy(operand2, operand1);
			}
			break;
		case '+':
			context = new Context(new OperationAdd());
			calculated = context->executeStrategy(operand2, operand1);
			break;
		case '-':
			context = new Context(new OperationSubstract());
			calculated = context->executeStrategy(operand2, operand1);
			break;
		default:
			try {
				throwException(2);
			}
			catch (string& s) {
				cout << s << endl;
				exit(0);
			}
		}
		valueStack.push(calculated);
	}

public:
	int evaluate(string aPostfixExpression) {
		char token;
		int current = 0;
		while (current < aPostfixExpression.length()) {
			token = aPostfixExpression.at(current);
			if (isdigit(token)) { // 숫자
				char* operandCharArray = (char*)calloc(aPostfixExpression.length() + 1, sizeof(char));
				int p = 0;
				do {
					*(operandCharArray + (p++)) = token;
					current++;
					token = aPostfixExpression.at(current);
				} while (token != ' ');
				int tokenValue = stoi(operandCharArray);
				free(operandCharArray);
				valueStack.push(tokenValue);
			}
			else if (token == ' ') {
				current++;
				continue;
			}
			else { // 연산자
				this->executeBinaryOperator(token);
				current++;
			}
		}
		int r = valueStack.top();
		valueStack.pop();
		return r;
	}
};

class Calculator {
	stack<char> operatorStack;
	string infixExpression;
	string postfixExpression;
	PostfixCalculator* postfixCalculator = new PostfixCalculator();

	int inComingPrecedence(char& aToken) {
		switch (aToken) {
		case '(': return 20;
		case ')': return 19;
		case '*': return 13;
		case '/': return 13;
		case '+': return 12;
		case '-': return 12;
		default:
			return -1; // 알 수 없는 연산자
		}
	}

	int inStackPrecedence(char& aToken) {
		switch (aToken) {
		case '(': return 0;
		case ')': return 19;
		case '*': return 13;
		case '/': return 13;
		case '+': return 12;
		case '-': return 12;
		default:
			return -1; // 알 수 없는 연산자
		}
	}

	void infixToPostfix() {
		char* postfixExpressionArray = (char*)calloc(2 * (infixExpression.length() + 1), sizeof(char));
		char currentToken, poppedToken, topToken;
		int p = 0;
		int i = 0;
		while (i < infixExpression.length()) {
			currentToken = infixExpression.at(i);
			if (isdigit(currentToken)) { // currentToken 이 operand
				// "0x", "0b" 형태 if
				if (currentToken == '0') {
					char* hexOrBinaryDigitArray = (char*)calloc(infixExpression.length() + 1, sizeof(char));
					int hexOrBinaryDigit;
					int ap = 0;
					i++;
					if (i < infixExpression.length()) {
						currentToken = infixExpression.at(i);
					}
					else {
						*(postfixExpressionArray + (p++)) = currentToken;
						*(postfixExpressionArray + (p++)) = ' ';
						continue;
					}
					if (currentToken == 'x') {
						i++;
						currentToken = infixExpression.at(i);
						do {
							*(hexOrBinaryDigitArray + (ap++)) = currentToken;
							i++;
							if (i < infixExpression.length()) {
								currentToken = infixExpression.at(i);
							}
							else break;
						} while (isdigit(currentToken) || (('A' <= currentToken) && (currentToken <= 'F')) || (('a' <= currentToken) && (currentToken <= 'f')));
						hexOrBinaryDigit = strtol(hexOrBinaryDigitArray, NULL, 16);
					}
					else if (currentToken == 'b') {
						i++;
						currentToken = infixExpression.at(i);
						do {
							*(hexOrBinaryDigitArray + (ap++)) = currentToken;
							i++;
							if (i < infixExpression.length()) {
								currentToken = infixExpression.at(i);
							}
							else break;
						} while (isdigit(currentToken));
						hexOrBinaryDigit = strtol(hexOrBinaryDigitArray, NULL, 2);
					}
					else {
						continue;
					}
					string s = to_string(hexOrBinaryDigit);
					char* carr = (char*)s.c_str();
					ap = 0;
					hexOrBinaryDigitArray = (char*)memset(hexOrBinaryDigitArray, 0, _msize(hexOrBinaryDigitArray));
					for (int j = 0; j < strlen(carr); j++) {
						*(hexOrBinaryDigitArray + (ap++)) = carr[j];
					}
					ap = 0;
					while (*(hexOrBinaryDigitArray + ap) != NULL) {
						*(postfixExpressionArray + (p++)) = *(hexOrBinaryDigitArray + (ap++));
					}
					*(postfixExpressionArray + (p++)) = ' ';
					free(hexOrBinaryDigitArray);
				}
				else {
					do {
						*(postfixExpressionArray + (p++)) = currentToken;
						i++;
						if (i < infixExpression.length()) {
							currentToken = infixExpression.at(i);
						}
						else break;
					} while (isdigit(currentToken));
					*(postfixExpressionArray + (p++)) = ' ';
				}
			}
			else { // currentToken 이 operator
				i++;
				if (currentToken == ')') { // ")"
					if (!operatorStack.empty()) {
						poppedToken = operatorStack.top();
						operatorStack.pop();
						while (!operatorStack.empty() && poppedToken != '(') {
							*(postfixExpressionArray + (p++)) = poppedToken;
							*(postfixExpressionArray + (p++)) = ' ';
							poppedToken = operatorStack.top();
							operatorStack.pop();
						}
						if (poppedToken == '(') {
							continue;
						}
					}
					if (operatorStack.empty()) {
						try {
							throwException(5);
						}
						catch (string& s) {
							cout << s << endl;
							exit(0);
						}
					}
				}
				else { // 일반 연산자
					int inComingPrecedence = this->inComingPrecedence(currentToken);
					if (inComingPrecedence < 0) {
						try {
							throwException(2);
						}
						catch (string& s) {
							cout << s << endl;
							exit(0);
						}
					}
					if (!operatorStack.empty()) {
						topToken = operatorStack.top();
						while (this->inStackPrecedence(topToken) >= inComingPrecedence) {
							poppedToken = topToken;
							operatorStack.pop();
							*(postfixExpressionArray + (p++)) = poppedToken;
							*(postfixExpressionArray + (p++)) = ' ';
							if (!operatorStack.empty()) {
								topToken = operatorStack.top();
							}
							else {
								break;
							}
						}
					}
					operatorStack.push(currentToken);
				}
			}
		}
		while (!operatorStack.empty()) {
			poppedToken = operatorStack.top();
			operatorStack.pop();
			if (poppedToken == '(') {
				try {
					throwException(1);
				}
				catch (string& s) {
					cout << s << endl;
					exit(0);
				}
			}
			*(postfixExpressionArray + (p++)) = poppedToken;
			*(postfixExpressionArray + (p++)) = ' ';
		}
		string str(postfixExpressionArray);
		postfixExpression = str;
		free(postfixExpressionArray);
	}

public:
	int evaluate(string anInfixExpression) {
		this->infixExpression = anInfixExpression;
		this->infixToPostfix();
		return postfixCalculator->evaluate(postfixExpression);
	}
};

int main() {
	Calculator* calculator = new Calculator();

	string infixExpression;
	cin >> infixExpression;

	int result = calculator->evaluate(infixExpression);
	cout << result << endl;
}