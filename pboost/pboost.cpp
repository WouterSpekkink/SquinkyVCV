#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <string>

class ProcessNameAndHandle
{
public:
    std::string name;
    HANDLE handle = INVALID_HANDLE_VALUE;
};

ProcessNameAndHandle  getProcessNameAndHandle(DWORD pid);
bool setRealtimePriority(HANDLE pHandle);
bool enablePrivilege(HANDLE hProcess, const char * privilege);
std::string GetLastErrorAsString();


/*
BOOL WINAPI DuplicateHandle(
  _In_  HANDLE   hSourceProcessHandle,
  _In_  HANDLE   hSourceHandle,
  _In_  HANDLE   hTargetProcessHandle,
  _Out_ LPHANDLE lpTargetHandle,
  _In_  DWORD    dwDesiredAccess,
  _In_  BOOL     bInheritHandle,
  _In_  DWORD    dwOptions
);
*/
int main(int, char**)
{

    printf("enter main\n");
    HANDLE pseudoHandle = GetCurrentProcess();
   
    // HANDLE realHandle = DuplicateHandle(pseudoHandle);
    HANDLE realHandle = INVALID_HANDLE_VALUE;
    bool dh = DuplicateHandle(pseudoHandle,
        pseudoHandle,
        pseudoHandle, 
        &realHandle,
        DUPLICATE_SAME_ACCESS,
        FALSE,
        DUPLICATE_SAME_ACCESS
        );


    printf("dupret %d pseude %x, real %x\n", dh,pseudoHandle, realHandle);
    bool b = enablePrivilege(realHandle, SE_DEBUG_NAME);
    printf("try set debug on us: %d\n", b);
    if (!b) {
        printf("can't get right from system. Try running as admin\n");
        return -1;
    }

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
        printf("enum proc failed\n");
        return 1;
    }
   

    // Calculate how many process identifiers were returned.
    cProcesses = cbNeeded / sizeof(DWORD);
     printf("num found %d procs\n", cbNeeded);

    // find the pid for Rack.
    HANDLE rackProcessHandle = INVALID_HANDLE_VALUE;
    std::string rackName("Rack.exe");
    for (i = 0; i < cProcesses; i++) {
        if (aProcesses[i] != 0) {
            //PrintProcessNameAndID( aProcesses[i] );
            ProcessNameAndHandle proc = getProcessNameAndHandle(aProcesses[i]);
            if (proc.name == rackName) {
                rackProcessHandle = proc.handle;
            } else {
                CloseHandle(proc.handle);
            }
        }
    }
    printf("rack pid = %d\n", rackProcessHandle);
    if (rackProcessHandle == INVALID_HANDLE_VALUE) {
        printf("could not find rack process\n");
        return -1;
    }

    bool bSet = setRealtimePriority(rackProcessHandle);
    CloseHandle(rackProcessHandle);
}



/**
 *
 */
ProcessNameAndHandle getProcessNameAndHandle(DWORD processID)
{
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.
    // was (PROCESS_QUERY_INFORMATION |  PROCESS_VM_READ,
    // We want PROCESS_SET_INFORMATION, but we can't get it yet
   
   // HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |  PROCESS_VM_READ,
    //    FALSE, processID);
   HANDLE hProcess = OpenProcess(PROCESS_SET_INFORMATION |PROCESS_QUERY_INFORMATION | PROCESS_VM_READ ,
        FALSE, processID);

// Get the process name
    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded;

        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
            &cbNeeded)) {
            GetModuleBaseName(hProcess, hMod, szProcessName,
                sizeof(szProcessName) / sizeof(TCHAR));
        } else printf("could not enum proc\n");
    } else {
        printf("process open failed: %s\n", GetLastErrorAsString().c_str());
        return {};
    }

    printf("this one opened: %s pid=%d\n", szProcessName, processID);
    // Release the handle to the process.
   // CloseHandle(hProcess);
    return {szProcessName, hProcess};
}


//Returns the last Win32 error, in string format. Returns an empty string if there is no error.//Returns 
std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

bool setClassRealtime(HANDLE h) {
    auto x = GetPriorityClass(h);
    auto b = SetPriorityClass(h, REALTIME_PRIORITY_CLASS);
    auto c = GetPriorityClass(h);

    printf("setrealtime(%d) %d, %d, %d\n", REALTIME_PRIORITY_CLASS, x, b, c);
    if (!b) {
        printf("SetP call failed with %d (%s)\n", GetLastError(), GetLastErrorAsString().c_str());
    }

    return c == REALTIME_PRIORITY_CLASS;
}


bool enablePrivilege(HANDLE hProcess, const char * privilege)
{
    printf("called ep with %s\n", privilege);
 struct
    {
        DWORD Count;
        LUID_AND_ATTRIBUTES Privilege[1];
    } Info;

    HANDLE Token;
    BOOL Result;

    // Open the token.

    Result = OpenProcessToken(hProcess,
        TOKEN_ADJUST_PRIVILEGES,
        &Token);

    if (Result != TRUE) {
        printf("ep Cannot open process token.\n");
        return FALSE;
    }

    // Enable or disable?

    Info.Count = 1;
    if (true) {
        Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
    } else {
        Info.Privilege[0].Attributes = 0;
    }

    // Get the LUID.

    Result = LookupPrivilegeValue(NULL,
      //  SE_LOCK_MEMORY_NAME,
        privilege,
        &(Info.Privilege[0].Luid));

    if (Result != TRUE) {
        printf("ep Cannot get privilege for %s.\n", privilege);
        return FALSE;
    }

    // Adjust the privilege.

    Result = AdjustTokenPrivileges(Token, FALSE,
        (PTOKEN_PRIVILEGES) &Info,
        0, NULL, NULL);

    CloseHandle(Token); 

// Check the result.

    if (Result != TRUE) {
        printf("ep Cannot adjust token privileges (%u)\n", GetLastError());
        return FALSE;
    } else {
        if (GetLastError() != ERROR_SUCCESS) {
            printf("getlasterror = %d, %s\n", GetLastError(), GetLastErrorAsString().c_str());
            printf("Cannot enable the %s privilege; ", privilege
            );
            printf("please check the local policy.\n");
            return FALSE;
        }
    }

    return TRUE;
}

   

/**
 *
 */
bool setRealtimePriority(HANDLE hRackProcess)
{

 //  hack(std::string("rack"), hRackProcess);
 //  hack(std::string("me"), GetCurrentProcess());


#if 0
    printf("will use current for debugging\n");
    hProcess = GetCurrentProcess();
    HANDLE hAdminProcess = GetCurrentProcess();
#elif 0
    printf("using Rack.exe, self as admin\n");
    HANDLE hAdminProcess = GetCurrentProcess();
#else
    printf("using Rack.exe, and as admin\n");
    HANDLE hAdminProcess = hRackProcess;
#endif


    // Let's try asking for privilege for ourselves.
    // Maybe then we can raise Rack
   

 auto set = setClassRealtime(hRackProcess);
printf("set pri class at start ret %d\n", set);
if (set) {
    printf("will not do all the fancy stuff since it worked\n");
    return true;
}

    struct
    {
        DWORD Count;
        LUID_AND_ATTRIBUTES Privilege[1];
    } Info;

    HANDLE Token;
    BOOL Result;

    // Open the token.

    Result = OpenProcessToken(hAdminProcess,
        TOKEN_ADJUST_PRIVILEGES,
        &Token);

    if (Result != TRUE) {
        printf("Cannot open process token.\n");
        return FALSE;
    }

    // Enable or disable?

    Info.Count = 1;
    if (true) {
        Info.Privilege[0].Attributes = SE_PRIVILEGE_ENABLED;
    } else {
        Info.Privilege[0].Attributes = 0;
    }

    // Get the LUID.

    Result = LookupPrivilegeValue(NULL,
      //  SE_LOCK_MEMORY_NAME,
        SE_INC_BASE_PRIORITY_NAME,
        &(Info.Privilege[0].Luid));

    if (Result != TRUE) {
        printf("Cannot get privilege for %s.\n", SE_INC_BASE_PRIORITY_NAME);
        return FALSE;
    }

    // Adjust the privilege.

    Result = AdjustTokenPrivileges(Token, FALSE,
        (PTOKEN_PRIVILEGES) &Info,
        0, NULL, NULL);

// Check the result.

    if (Result != TRUE) {
        printf("Cannot adjust token privileges (%u)\n", GetLastError());
        return FALSE;
    } else {
        if (GetLastError() != ERROR_SUCCESS) {
            printf("getlasterror = %d, %s\n", GetLastError(), GetLastErrorAsString().c_str());
            printf("Cannot enable the SE_INC_BASE_PRIORITY_NAME privilege; ");
            printf("please check the local policy.\n");
            return FALSE;
        }
    }

    CloseHandle(Token);

    set = setClassRealtime(hRackProcess);
    printf("set pri after class ret %d\n", set);

    return TRUE;
}
