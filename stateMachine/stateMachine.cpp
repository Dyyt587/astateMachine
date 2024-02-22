// stateMachine.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "aStateMachine.h"
int main()
{
    std::cout << "Hello World!\n";
    aStateMachine* sm = new aStateMachine();
    aState* s1 = new aState("s1", []() {std::cout << "s1 excution" << std::endl; },[](){std::cout << "s1 entry" << std::endl; }, []() {std::cout << "s1 exit" << std::endl; });
    aState* s2 = new aState("s2", []() {std::cout << "s2 excution" << std::endl; },[](){std::cout << "s2 entry" << std::endl; }, []() {std::cout << "s2 exit" << std::endl; });
    aState* s3 = new aState("s3", []() {std::cout << "s3 excution" << std::endl; },[](){std::cout << "s3 entry" << std::endl; }, []() {std::cout << "s3 exit" << std::endl; });
    aState* s4 = new aState("s4", []() {std::cout << "s4 excution" << std::endl; },[](){std::cout << "s4 entry" << std::endl; }, []() {std::cout << "s4 exit" << std::endl; });
    sm->addState(s1);
    sm->addState(s2);
    sm->addState(s3);
    sm->addState(s4);
    sm->addTransition(s1, s2, [](aStateMachine* sm) {std::cout << sm->current->name<< std::endl; return true; });
    sm->addTransition(s2, s3, [](aStateMachine* sm) {return true; });
    // sm->addTransition(s3, s4, [=](aStateMachine* sm) {return true; });
    // sm->addTransition(s4, 0,  [=](aStateMachine* sm) {return true; });
    sm->showStates();

    sm->start(s1);
    sm->update();
    sm->update();
    sm->update();
    sm->update();
    sm->update();

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
