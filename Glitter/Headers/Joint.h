//
//  Joint.h
//  FBXLoader
//
//  Created by Jeehoon Hyun on 17/07/2019.
//  Copyright ?2019 Jeehoon Hyun. All rights reserved.
//

#ifndef Joint_h
#define Joint_h

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Joint
{
public:
    std::string name;
    glm::mat4 boneOffset;
    std::vector<glm::mat4> animationMatrices;
};

#endif /* Joint_h */
