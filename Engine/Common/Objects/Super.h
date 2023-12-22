//
// Created by Remus on 22/12/2023.
//

#pragma once


class Super {
public:
    virtual ~Super() = default;

    virtual void Construct();
    virtual void Tick(float aDeltaTime);
    virtual void Cleanup();
};
