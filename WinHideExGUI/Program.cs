using System;
using System.Threading;
using System.Windows.Forms;
using WinHideExGUI.Properties;

namespace WinHideExGUI
{
    static class Program
    {
        // Our mutex
        private static Mutex mutex = null;

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            mutex = new Mutex(true, Resources.AppName, out bool createdNew);

            if (!createdNew)
            {
                MessageBox.Show("Another instance of the application is already running!");
                return;
            }

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new CustomApplicationContext(new MainForm()));
        }
    }
}