
// Handles class destructions since there is still no heap
extern "C" {
    void *__dso_handle = nullptr;
    int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
}