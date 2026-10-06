#pragma once

// Returns true when the bootstrap handed control to the hidden legacy backend.
// The caller should terminate the current ProjectAtum process in that case.
bool RunAceTRClientBootstrap(HINSTANCE hInstance);
