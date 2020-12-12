# Boring LLVM Code

### *2020.11.30*
* Write self-defined add LLVM IR generator
    * 将三元组信息 及 数据布局对象放入模块并创建 Module实例
    * 定义函数签名，参数及返回类型，FunctionType 及 SmallVector<Type*, 2> FuncTyArgs
    * 使用Function::Create()静态方法创建函数
    * 存储参数的Value指针，可使用函数参数迭代器获得
    * 使用entry标签(或值名称)创建第一个基本块(BasicBlock)，并存储指针
    * 在entry基本块中添加对应的IR指令，也可使用IRBuilder来构建IR指令
    * main函数中调用函数创建Module，并使用verifyModule()验证IR构造，通过WriteBitcodeToFile函数将模块的位码写入磁盘
### *2020.12.6*
* Add IR generator LLVM 10.0同书中示例3.4的差异还是非常大的，基本都改了一套接口，很多类的定义也完全变掉...总而言之，进行修改还是非常蛋疼的事...
* Pass类为实现代码优化的主要类，常见子类：ModulePass(作用于整个模块), FunctionPass(一次处理一个函数,需重载runOnFunction), BasicBlockPass(作用在每个基本块)，继承Pass子类写自定义pass后RegisterPass
* LLVM后端：通用API抽象后端任务并特化为不同平台后端；主要流程：指令选择(SelectionDAG)，指令调度(pre-register allocation and post-register allocation)，寄存器分配，代码输出；CodeGen, MC, TableGen and Target.

### *2020.12.10*
* Value is the class to represent a "Static Single Assignment(SSA) register" or “SSA value” in LLVM. The most distinct aspect of SSA values is that their value is computed as the related instruction executes, and it does not get a new value until (and if) the instruction re-executes.
* The *Builder* object is a helper object that makes it easy to generate LLVM instructions. Instances of the *IRBuilder* class template keep track of the current place to insert instructions and has methods to create new instructions.
* The *Module* is an LLVM construct that contains functions and global variables. 
* In the LLVM IR, *numeric constants* are represented with the *ConstantFP* class, which holds the numeric value in an APFloat internally (APFloat has the capability of holding floating point constants of Arbitrary Precision). 
* The LLVM IRBuilder knows where to insert the newly created instruction, all you have to do is specify what instruction to create (e.g. with *CreateFAdd*), which operands to use (L and R here) and optionally provide a name for the generated instruction.
* LLVM instructions are constrained by strict rules: for example, the Left and Right operators of an add instruction must have the *same type*, and the result type of the add must *match* the operand types. 
* Once we have the function to call, we recursively codegen each argument that is to be passed in, and create an LLVM call instruction.
* The call to *FunctionType::get* creates the *FunctionType*. Note that *Types* in LLVM are uniqued just like *Constants* are, so you don’t “new” a type, you “get” it. *Function::Create* creates the IR function. “external linkage” means that the function may be defined outside the current module and/or that it is callable by functions outside the module.
* *Module::getFunction* search Module’s symbol table for an existing version of this function.
*  Basic blocks in LLVM are an important part of functions that define the Control Flow Graph. Use *BasicBlock::Create* to create the basic block and tell insert point to the IRBuilder.