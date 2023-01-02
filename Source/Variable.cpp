#include <Variable.hpp>

Variable::Variable() {}

Variable::Variable(std::string name) {
    this->name = name;
    this->value = value;
}

std::string Variable::GetName() {
    return this->name;
}