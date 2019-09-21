﻿using demo;
using Test.RdCross.Util;

namespace Test.RdCross
{
    // ReSharper disable once UnusedMember.Global
    internal class CrossTestCsClientTask : CrossTestCsClientBase
    {
        public override void Start(string[] args)
        {
            Before(args);
            
            Queue(() =>
            {
                var demoModel = new DemoModel(ModelLifetime, Protocol);
                
                demoModel.Call.Set(c => c.ToString());
                
                demoModel.Callback.Start("Csharp").Result.Advise(ModelLifetime, 
                    result =>
                    {
                        Printer.PrintAnyway(nameof(demoModel.Callback), result);

                        Finished = true;
                    });
            });
            
            After();
        }
    }
}