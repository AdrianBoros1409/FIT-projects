using System;
using System.Globalization;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using Fitter.App.ViewModels;
using Fitter.App.Views;

namespace Fitter.App
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public App()
        {
            string language;
            try
            {
                using (StreamReader sr = new StreamReader("language.txt"))
                {
                    language = sr.ReadToEnd();
                }
            }
            catch (Exception e)
            {
                language = "en";
            }

            try
            {
                Thread.CurrentThread.CurrentCulture = new CultureInfo(language);
                Thread.CurrentThread.CurrentUICulture = new CultureInfo(language);
            }
            catch
            {
                Thread.CurrentThread.CurrentCulture = new CultureInfo("en");
                Thread.CurrentThread.CurrentUICulture = new CultureInfo("en");
            }
            
        }
    }
}
