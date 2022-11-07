export void RotatorToQuat(const uniform float rotator[3], uniform float quat[4])
{
    uniform float compSin[3]; // Store as pitch yaw roll
    uniform float compCos[3]; // Store as pitch yaw roll

    for (uniform int index = 0; index < 3; ++index)
    {
        sincos(rotator[index] * PI / 360.f, &compSin[index], &compCos[index]);
        print("%, %, sin: %, cos: %", programIndex, programCount, compSin[index], compCos[index]);
    }

    quat[0] = compCos[2] * compSin[0] * compSin[1] - compSin[2] * compCos[0] * compCos[1];
    quat[1] = -compCos[2] * compSin[0] * compCos[1] - compSin[2] * compCos[0] * compSin[1];
    quat[2] = compCos[2] * compCos[0] * compSin[1] - compSin[2] * compSin[0] * compCos[1];
    quat[3] = compCos[2] * compCos[0] * compCos[1] + compSin[2] * compSin[0] * compSin[1];
}

export void RotatorToQuatFast(const uniform float rotator[3], uniform float quat[4])
{
    uniform float compSin[4]; // Store as pitch yaw roll
    uniform float compCos[4]; // Store as pitch yaw roll

    for (uniform int i = 0; i < 3; i += programCount) 
    {
        int index = i + programIndex;
        float sin, cos;
        sincos(rotator[index] * PI / 360.f, &sin, &cos);
        compSin[index] = sin;
        compCos[index] = cos;
        //print("%, sin: %, cos: %", index, compSin[index], compCos[index]);
    }
    // foreach (index = 0 ... 3) 
    // {
    //     //int index = i + programIndex;
    //     float sin, cos;
    //     sincos(rotator[index] * PI / 360.f, &sin, &cos);
    //     compSin[index] = sin;
    //     compCos[index] = cos;
    //     print("%, sin: %, cos: %", programIndex, compSin[index], compCos[index]);
    // }
    // foreach_active(index)
    // {
    //     if (index < 3)
    //     {
    //         sincos(rotator[index] * PI / 360.f, &compSin[index], &compCos[index]);
    //     }
    // }

    quat[0] = compCos[2] * compSin[0] * compSin[1] - compSin[2] * compCos[0] * compCos[1];
    quat[1] = -compCos[2] * compSin[0] * compCos[1] - compSin[2] * compCos[0] * compSin[1];
    quat[2] = compCos[2] * compCos[0] * compSin[1] - compSin[2] * compSin[0] * compCos[1];
    quat[3] = compCos[2] * compCos[0] * compCos[1] + compSin[2] * compSin[0] * compSin[1];
}