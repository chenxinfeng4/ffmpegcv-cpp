#!/bin/bash
# FFmpeg command to create a 640x480 blue video file for testing
#!/bin/bash

ffmpeg -y -f lavfi -i "testsrc=size=640x480:duration=20" -r 30 -c:v libx264 -pix_fmt yuv420p input.mp4
