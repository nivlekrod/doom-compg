#version 120

uniform sampler2D uTexture;
uniform float uTime;
uniform float uStrength;
uniform vec2  uScroll;
uniform float uIntensity;

varying vec2 vTexCoord;

void main()
{
    vec2 uv = vTexCoord + uScroll * uTime;

    // múltiplas ondas de fogo
    float w1 = sin(uv.y * 30.0 - uTime * 5.0);
    float w2 = cos(uv.x * 25.0 + uTime * 4.0);
    float w3 = sin((uv.x + uv.y) * 20.0 - uTime * 6.0);
    
    float distort = (w1 + w2 + w3) * 0.33;
    
    // distorção vertical mais forte (fogo sobe)
    vec2 uvDistorted = uv + uStrength * vec2(distort * 0.015, distort * 0.05);

    vec4 baseColor = texture2D(uTexture, uvDistorted);

    // gradiente de fogo: amarelo -> laranja -> vermelho
    float gradient = 1.0 - vTexCoord.y;
    vec3 fireBase = mix(vec3(1.0, 0.2, 0.0), vec3(1.0, 0.9, 0.2), gradient);
    
    vec3 color = mix(baseColor.rgb, fireBase, 0.7);

    // pontos de chama intensa
    float flamePulse = sin(uTime * 8.0 + uv.y * 40.0) * 0.5 + 0.5;
    float flameIntensity = pow(abs(distort), 1.5) * flamePulse;
    vec3 flameGlow = vec3(1.3, 0.8, 0.3);
    
    color += flameIntensity * flameGlow * uIntensity;

    // escurece nas bordas (fogo desvanece)
    float fade = smoothstep(0.0, 0.3, vTexCoord.y) * smoothstep(1.0, 0.7, vTexCoord.y);
    color *= fade;

    color = clamp(color, 0.0, 1.0);

    gl_FragColor = vec4(color, baseColor.a * fade);
}
