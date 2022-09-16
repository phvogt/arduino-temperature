#include "config.h"
#include "program.h"

std::unique_ptr<arduino_temp::Program> program_ {};

void setup() { 
    arduino_temp::Config config;
    program_ = std::make_unique<arduino_temp::Program>(config);
    program_->setup(); 
}

void loop() { program_->loop(); }