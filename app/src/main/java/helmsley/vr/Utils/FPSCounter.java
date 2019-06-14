package helmsley.vr.Utils;

import helmsley.vr.UIsController;

public class FPSCounter {
    static long startTime = System.nanoTime();
    static int frames = 0;

    public static void logFrame() {
        frames++;
        if(System.nanoTime() - startTime >= 1000000000) {
            UIsController.FPSlabel.setText(String.format("%2.2f FPS", frames));
            frames = 0;
            startTime = System.nanoTime();
        }
    }
}
