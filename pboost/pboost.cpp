#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include <string>



class ProcessNameAndId
{
public:
    std::string name;
    DWORD pid=0;
};
//
//void PrintProcessNameAndID( DWORD processID );
ProcessNameAndId getProcessNameAndId(DWORD pid);




int main(int, char**) 
{
    printf("Here I am in main\n");

    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
    {
        printf("enum proc failed\n");
        return 1;
    }

    printf("there are %d procs\n", cbNeeded);
    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the name and process identifier for each process.

    DWORD pid = 0;
    std::string rackName("Rack.exe");
    for ( i = 0; i < cProcesses; i++ )
    {
        if( aProcesses[i] != 0 )
        {
            //PrintProcessNameAndID( aProcesses[i] );
           ProcessNameAndId proc = getProcessNameAndId(aProcesses[i]);
            if (proc.name == rackName) {
                pid = proc.pid;
            }

        }
    }
    printf("pid = %d\n", pid);
}

ProcessNameAndId getProcessNameAndId( DWORD processID )
{
    printf("in get %d\n", processID);
    TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");

    // Get a handle to the process.

    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ,
                                   FALSE, processID );

    // Get the process name.
    printf("hproc = %d\n", hProcess);
    if (NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;

        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
             &cbNeeded) )
        {
            GetModuleBaseName( hProcess, hMod, szProcessName, 
                               sizeof(szProcessName)/sizeof(TCHAR) );
        }
    }

    // Print the process name and identifier.
    const char *p = szProcessName;

    printf("found %s,%d,%d\n", szProcessName, processID, hProcess );
    //printf( TEXT("%s  (PID: %u)\n"), szProcessName, processID );

    // Release the handle to the process.

    CloseHandle( hProcess );
    ProcessNameAndId ret = {szProcessName, processID};
    return ret;
}
