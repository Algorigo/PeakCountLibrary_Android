package com.algorigo.peakcount

class PeakCount {

    /**
     * A native method that is implemented by the 'peakcount' native library,
     * which is packaged with this application.
     */

    companion object {
        // Used to load the 'peakcount' library on application startup.
        init {
            System.loadLibrary("peakcount")
        }

        @JvmStatic
        external fun peakCount(values: List<Double>, substractAlpha: Double, emaAlpha: Double): Int
    }
}
