#version 400 core
out vec4 FragColor;

uniform vec2 windowSize;
uniform float time;
uniform dvec2 mousePos;
uniform dvec2 offset;
uniform double zoom;

void main() {
    vec2 coords = gl_FragCoord.xy / windowSize - 0.5;
    float asprectRatio = windowSize.x / windowSize.y;
    if (asprectRatio >= 1) {
        coords.x = coords.x * asprectRatio;
    } else {
        coords.y = coords.y / asprectRatio;
    }
    vec2 uv = coords * 2.0;
    int maxIterations = int(log(log(float(zoom + 9)) / 2.0) * 175);
//    int maxIterations = 1000;
    dvec2 z;
    dvec2 c = (coords / zoom - offset) * 2.0;
    double x = 0;
    double y = 0;
    int i = 0;
    float smoothI = 1.0f;
    for (; i < maxIterations; i++) {
        x = (z.x * z.x - z.y * z.y) + c.x;
        y = (z.y * z.x + z.x * z.y) + c.y;
        float sqSum = float(x * x + y * y);
        if (sqSum > 40) {
            smoothI = i + 1.0 - log2(log(sqSum));
            break;
        }
        z.x = x;
        z.y = y;
    }
    vec3 color = cos(vec3(1.1, 1.2, 1.3) * sqrt(smoothI * 2.0));
    FragColor = vec4(color, 1.0);
}