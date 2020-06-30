

struct Particle
{
    float4 pos;
    float4 velocity;
    float4 acceleration;
    float4 force;
    float4 parentPos;
    float4 distance;
    float4 offset;
    float isRoot;
    float mass;
    float radius;
    float isIN;
};

cbuffer ConstantBuffer : register(b0)
{
    float4 Headpos;
    float4 SphereObject;
    float4 SphereVelocity;
    float Sphereraduis;
    float MaxParticles;
    float hairLength;
    float deltaTime;
}



RWStructuredBuffer<Particle> particlesIn;

RWStructuredBuffer<Particle> particlesOut;

bool CheckCollisonDetection(inout Particle p)
{
   
    float3 distancePos = SphereObject.xyz - p.pos.xyz;
    float distance = length(distancePos);
    if (distance < p.radius + Sphereraduis)
    {
        return true;
    }  
    else
        return false;   
}

void CalculateCoefficientResolution(inout Particle p)
{
    float e1 = 0.4f;
    float e = e1 + 1.0f / 2;
    
    float Object1Mass = p.mass;
    float Object2Mass = 20.0f;
  
    float3 Object1InitialVelocity = p.velocity.xyz;
    float3 Object2InitialVelocity = SphereVelocity.xyz;
    
    float3 temp1 = Object1InitialVelocity * (Object1Mass - (Object2Mass * e));
    float3 temp2 = Object2InitialVelocity * Object2Mass * (e1 + e);
    
    float3 Object1FinalVelocity = (temp1 + temp2) / (Object1Mass + Object2Mass);
    
    temp1 = Object1InitialVelocity * Object1Mass;
    temp2 = Object2InitialVelocity * Object2Mass;
    
    float3 temp3 = Object1FinalVelocity * Object1Mass;
    float3 Object2FinalVelocity = (temp1 + temp2 - temp3) / Object2Mass;
    
    float3 Object1Normal = p.pos.xyz - SphereObject.xyz;
    normalize(Object1Normal);
    
   
    float3 Object1RelativeVelocity = Object1Normal * dot(Object1Normal, Object1FinalVelocity);
    
    p.velocity.xyz = Object1RelativeVelocity;

}

void SphereAndSphereCollisionResolution(inout Particle p)
{
    float totalRadius = p.radius + Sphereraduis;
    float3 firstPosition = p.pos.xyz;
    float3 secondPosition = SphereObject;
    float3 forDistance = firstPosition - secondPosition;
    
    float distance = length(forDistance);
    float depth = totalRadius - distance;
    
    float3 direction = firstPosition - secondPosition;
    
    normalize(direction);
    
    firstPosition = p.pos.xyz + direction * depth;
    
    p.pos.x = firstPosition;
    
    CalculateCoefficientResolution(p);

}


[numthreads(64, 1, 1)]
void CSMain(uint3 DTid : SV_DispatchThreadID)
{
    
    float k = 60.0f; //spring stiffness
    float damping = 9.5f;
    float3 gravity = float3(0.0f, -9.8f, 0.0f);
    float3 windResitance = float3(0.0f, 0.0f, 0.0);
    float timeStep = 0.048f;
    //if (DTid.x >= MaxParticles)
    //    return;
    
    Particle particle = particlesIn[DTid.x];
    
    particlesOut[DTid.x] = particle;
    if (particle.isRoot == 1)
    {
        particle.pos.xyz = Headpos.xyz + particle.offset.xyz;
        particlesOut[DTid.x] = particle;
        
        for (uint i = DTid.x + 1; i < DTid.x + hairLength; i++)
        {
            Particle Parent = particlesIn[i - 1];
            Particle P = particlesIn[i];
            
           float3 Springforce = -k * (P.pos.xyz - Parent.pos.xyz);
           float3 gravityforce = gravity * P.mass;
           float3 DampingForce = damping * P.velocity.xyz;
                
           P.force.xyz += Springforce; + gravityforce + windResitance;
           P.force.xyz -= DampingForce;
           P.force.x += windResitance.x;
           P.acceleration.xyz = P.force.xyz / P.mass;
           P.velocity.xyz += P.acceleration.xyz * deltaTime;
           P.pos.xyz += P.velocity.xyz * deltaTime;

           P.force.xyz = 0.0f;
        
    
            if (CheckCollisonDetection(P))
            {
               // P.isIN = 1.0f;
                SphereAndSphereCollisionResolution(P);

            }
            
            if (distance(P.pos.y, Parent.pos.y) < 0.001f || distance(P.pos.y, Parent.pos.y) > 0.001f)
            {
                P.pos.y -= 0.09f;
            }
        
            float4 delta = Parent.pos - P.pos;
      
            float _distance = delta.x * delta.x + delta.y * delta.y + delta.z * delta.z;
      
            _distance = sqrt(_distance);
        
            float dest_length = 0.4f;
        
            delta *= 0.2 * (((dest_length) - _distance) / _distance);
        
            P.pos -= delta;
            Parent.pos -= delta;
          
            particlesOut[i] = P;
        }
    }
    
    
 }
