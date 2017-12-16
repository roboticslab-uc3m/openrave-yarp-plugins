// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "FakeControlboard.hpp"

// ----------------------------------------------------------------------------

void roboticslab::FakeControlboard::setEncRaw(const int Index, const double Position) {
    // printf("[SharedArea] setEncRaw.\n");
    encRawMutex.wait();
    encRaw[Index] = Position;
    encRawMutex.post();
}

// ----------------------------------------------------------------------------

void roboticslab::FakeControlboard::setEncsRaw(const std::vector<double> & Positions) {
    encRawMutex.wait();
    encRaw = Positions;
    encRawMutex.post();
}

// ----------------------------------------------------------------------------

double roboticslab::FakeControlboard::getEncRaw(const int Index) {
    // printf("[SharedArea] getEncRaw.\n");
    double Position;
    encRawMutex.wait();
    Position = encRaw[Index];
    encRawMutex.post();
    return Position;
}

// ----------------------------------------------------------------------------

std::vector<double> roboticslab::FakeControlboard::getEncsRaw() {
    std::vector<double> Positions;
    encRawMutex.wait();
    Positions = encRaw;
    encRawMutex.post();
    return Positions;
}

// ----------------------------------------------------------------------------

double roboticslab::FakeControlboard::getEncExposed(const int Index) {
    double RawPosition = getEncRaw(Index);
    return RawPosition / encRawExposed[Index];
}

// ----------------------------------------------------------------------------

std::vector<double> roboticslab::FakeControlboard::getEncsExposed() {
    std::vector<double> RawPositions = getEncsRaw();
    for (unsigned int i = 0; i < RawPositions.size(); i++) {
        RawPositions[i] /= encRawExposed[i];
    }
    return RawPositions;
}

// ----------------------------------------------------------------------------
