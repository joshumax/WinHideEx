using Microsoft.Win32;
using Reloaded.Injector;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using WinHideExGUI.Properties;

namespace WinHideExGUI
{
    public partial class MainForm : Form
    {
        // Hidden status
        private bool hidden = false;

        // Registry stuff
        private RegistryKey RegKey = Registry.CurrentUser.CreateSubKey(@"SOFTWARE\WinHideEx");
        private RegistryKey Autorun = Registry.CurrentUser.OpenSubKey("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true);
        private readonly string RegInjectionTargets = "InjectionTargets";
        private readonly string RegRegularExpression = "RegularExpression";
        private readonly string RegHookSettings = "HookSettings";

        // The name of our DLLs to inject
        private readonly string DLL32Bit = "WinHideEx32.dll";
        private readonly string DLL64Bit = "WinHideEx64.dll";

        public MainForm()
        {
            this.InitializeComponent();
        }

        void Exit(object sender, EventArgs e)
        {
            // Hide tray icon, otherwise it will remain shown until user mouses over it
            this.NotifyIcon1.Visible = false;

            Application.Exit();
        }

        private string[] GetInjectionTargetsFromRegistry()
        {
            if (RegKey == null)
                return null;

            string res = (string)RegKey.GetValue(RegInjectionTargets);

            if (res == null)
                return null;

            return res.Split(';');
        }

        private string GetRegularExpressionFromRegistry()
        {
            string default_regex = "\\..*";
            if (RegKey == null)
                return default_regex;

            string res = (string)RegKey.GetValue(RegRegularExpression);

            // Load our default regex (hide all dotfiles)
            if (res == null)
                return default_regex;

            return res.Replace('|', '\n');
        }

        private bool GetEnabledStatusFromRegistry()
        {
            if (RegKey == null)
                return false;

            string res = (string)RegKey.GetValue(RegHookSettings);

            if (res == null)
                return false;

            return res == "enabled";
        }

        private void WriteSettingsToRegistry()
        {
            // Save all of our values back to the registry
            RegKey.SetValue(RegHookSettings, this.InjectionEnabled.Checked ? "enabled" : "disabled");
            RegKey.SetValue(RegRegularExpression, String.Join("|",
                this.RegexList.Text.Split(new string[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries)));

            var list = new List<string>();
            foreach (var item in this.InjectList.Items)
            {
                list.Add(item.ToString());
            }

            RegKey.SetValue(RegInjectionTargets, String.Join(";", list));

            // Autorun setting
            if (this.RunOnStartup.Checked)
                this.Autorun.SetValue(Resources.AppName, Application.ExecutablePath);
            else
                this.Autorun.DeleteValue(Resources.AppName, false);
        }

        private void LoadSettingsFromRegistry()
        {
            // Add our existing inject targets
            string[] targets = GetInjectionTargetsFromRegistry();
            if (targets != null)
            {
                foreach (string target in targets)
                    this.InjectList.Items.Add(target);
            }

            // Add our existing regexes
            this.RegexList.Text = GetRegularExpressionFromRegistry();

            // Add our existing inject enabled status
            this.InjectionEnabled.Checked = GetEnabledStatusFromRegistry();

            // Update our autorun checkbox
            this.RunOnStartup.Checked = this.Autorun.GetValue(Resources.AppName) != null;
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            this.NotifyIcon1.ContextMenu = new ContextMenu(new MenuItem[] {
                new MenuItem("Exit", Exit)
            });

            this.Hide();
            this.hidden = true;

            // Load our existing settings from the registry
            LoadSettingsFromRegistry();

            // Write back again to load sane defaults on first run
            WriteSettingsToRegistry();

            // Run our Injection worker
            InjectionWorker.RunWorkerAsync();
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);
            if (e.CloseReason == CloseReason.UserClosing)
            {
                e.Cancel = true;
                this.Hide();
            }
        }

        private void NotifyIcon1_Click(object sender, EventArgs e)
        {
            if (hidden)
            {
                this.StartPosition = FormStartPosition.Manual;
                this.Location = new Point(Screen.PrimaryScreen.WorkingArea.Width - this.Width,
                    Screen.PrimaryScreen.WorkingArea.Height - this.Height);

                this.Show();
                this.hidden = false;
            }
            else
            {
                this.Hide();
                this.hidden = true;
            }
        }

        private void AddProgram_Click(object sender, EventArgs e)
        {
            OpenFileDialog open = new OpenFileDialog();
            open.InitialDirectory = @"C:\";
            open.RestoreDirectory = true;
            open.Title = "Add an Application to Inject";
            open.Filter = "Application files (*.exe)|*.exe";

            if (open.ShowDialog() == DialogResult.OK)
            {
                if (InjectList.Items.Contains(open.FileName))
                    return;
                else
                    InjectList.Items.Add(open.FileName);
            }
        }

        private void DeleteProgram_Click(object sender, EventArgs e)
        {
            if (InjectList.SelectedIndex == -1)
                return;

            InjectList.Items.RemoveAt(InjectList.SelectedIndex);
        }

        private void InjectionWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            // Loop through all the targets every 1 second and see
            // if they need to be injected with our WinHideEx DLL
            while (true)
            {
                System.Threading.Thread.Sleep(1000);

                string[] all_to_inject = GetInjectionTargetsFromRegistry();
                if (all_to_inject == null)
                    break;

                foreach (var process in Process.GetProcesses())
                {
                    foreach (var to_inject in all_to_inject)
                    {
                        // Check if the process file name is in our list, and if so, inject our DLL
                        if (GetProcessFilename(process) == to_inject)
                            InjectDLLIntoProcess(process);
                    }
                }
            }
        }

        private void InjectDLLIntoProcess(Process process)
        {
            // Don't re-inject into a process we've already injected
            foreach (var module in CollectModules(process))
            {
                if (module.ModuleName == this.DLL32Bit || module.ModuleName == this.DLL64Bit)
                    return;
            }

            // Actually inject the DLL now!
            Injector injector = new Injector(process);

            // SHould we load the 32 bit or 64 bit DLL in the target process?
            string DLL = Is64Bit(process) ? this.DLL64Bit : this.DLL32Bit;

            injector.Inject(Directory.GetCurrentDirectory() + Path.DirectorySeparatorChar + DLL);
            injector.Dispose();
        }

        private static bool Is64Bit(Process process)
        {
            if (!Environment.Is64BitOperatingSystem)
                return false;

            if (!IsWow64Process(process.Handle, out bool isWow64))
                throw new Win32Exception();

            return !isWow64;
        }

        [DllImport("kernel32.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool IsWow64Process([In] IntPtr process, [Out] out bool wow64Process);

        [Flags]
        private enum ProcessAccessFlags : uint
        {
            QueryLimitedInformation = 0x00001000
        }

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern bool QueryFullProcessImageName([In] IntPtr hProcess, [In] int dwFlags,
            [Out] StringBuilder lpExeName, ref int lpdwSize);

        [DllImport("kernel32.dll", SetLastError = true)]
        private static extern IntPtr OpenProcess(ProcessAccessFlags processAccess, bool bInheritHandle,
            int processId);

        String GetProcessFilename(Process p)
        {
            int capacity = 2000;
            StringBuilder builder = new StringBuilder(capacity);
            IntPtr ptr = OpenProcess(ProcessAccessFlags.QueryLimitedInformation, false, p.Id);

            if (!QueryFullProcessImageName(ptr, 0, builder, ref capacity))
            {
                return String.Empty;
            }

            return builder.ToString();
        }

        public List<Module> CollectModules(Process process)
        {
            List<Module> collectedModules = new List<Module>();

            IntPtr[] modulePointers = new IntPtr[0];

            // Determine number of modules
            if (!Native.EnumProcessModulesEx(process.Handle, modulePointers, 0, out int bytesNeeded, (uint)Native.ModuleFilter.ListModulesAll))
            {
                return collectedModules;
            }

            int totalNumberofModules = bytesNeeded / IntPtr.Size;
            modulePointers = new IntPtr[totalNumberofModules];

            // Collect modules from the process
            if (Native.EnumProcessModulesEx(process.Handle, modulePointers, bytesNeeded, out bytesNeeded, (uint)Native.ModuleFilter.ListModulesAll))
            {
                for (int index = 0; index < totalNumberofModules; index++)
                {
                    StringBuilder moduleFilePath = new StringBuilder(1024);
                    Native.GetModuleFileNameEx(process.Handle, modulePointers[index], moduleFilePath, (uint)(moduleFilePath.Capacity));

                    string moduleName = Path.GetFileName(moduleFilePath.ToString());
                    Native.ModuleInformation moduleInformation = new Native.ModuleInformation();
                    Native.GetModuleInformation(process.Handle, modulePointers[index], out moduleInformation, (uint)(IntPtr.Size * (modulePointers.Length)));

                    // Convert to a normalized module and add it to our list
                    Module module = new Module(moduleName, moduleInformation.lpBaseOfDll, moduleInformation.SizeOfImage);
                    collectedModules.Add(module);
                }
            }

            return collectedModules;
        }

        public class Native
        {
            [StructLayout(LayoutKind.Sequential)]
            public struct ModuleInformation
            {
                public IntPtr lpBaseOfDll;
                public uint SizeOfImage;
                public IntPtr EntryPoint;
            }

            internal enum ModuleFilter
            {
                ListModulesDefault = 0x0,
                ListModules32Bit = 0x01,
                ListModules64Bit = 0x02,
                ListModulesAll = 0x03,
            }

            [DllImport("psapi.dll")]
            public static extern bool EnumProcessModulesEx(IntPtr hProcess, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U4)][In][Out] IntPtr[] lphModule, int cb, [MarshalAs(UnmanagedType.U4)] out int lpcbNeeded, uint dwFilterFlag);

            [DllImport("psapi.dll")]
            public static extern uint GetModuleFileNameEx(IntPtr hProcess, IntPtr hModule, [Out] StringBuilder lpBaseName, [In][MarshalAs(UnmanagedType.U4)] uint nSize);

            [DllImport("psapi.dll", SetLastError = true)]
            public static extern bool GetModuleInformation(IntPtr hProcess, IntPtr hModule, out ModuleInformation lpmodinfo, uint cb);
        }

        public class Module
        {
            public Module(string moduleName, IntPtr baseAddress, uint size)
            {
                this.ModuleName = moduleName;
                this.BaseAddress = baseAddress;
                this.Size = size;
            }

            public string ModuleName { get; set; }
            public IntPtr BaseAddress { get; set; }
            public uint Size { get; set; }
        }

        private void SaveButton_Click(object sender, EventArgs e)
        {
            WriteSettingsToRegistry();
        }
    }
}