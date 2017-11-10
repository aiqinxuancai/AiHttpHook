# AiHttpHook
使用APIHOOK技术对Wininet.dll的相关函数进行Hook用于拦截当前进程的Http请求，应用于ToukenBrowser的3.5及更早的版本。

Use APIHook technology to perform Hooks on Wininet.dll functions(unicode) to intercept Http and https requests for the current process.

VC2008 dll

Quick Start (C#)
```csharp
        [DllImport("AiHttpHook.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int StartPageHook(IntPtr _dwNewLong);


        private delegate int HttpCallback(string path, string result, string send, string host);
        private static HttpCallback HttpPackCallback;


        public static void Init()
        {
            HttpPackCallback = new HttpCallback(JsonRoute);
            StartPageHook(Marshal.GetFunctionPointerForDelegate(HttpPackCallback);
        }


        /// <summary>
        /// recv func
        /// </summary>
        /// <param name="path">path</param>
        /// <param name="result">return data</param>
        /// <param name="send">post data</param>
        /// <param name="host">host</param>
        /// <returns></returns>
        public static int JsonRoute(string path, string result, string send, string host)
        {
            return 0;
        }
```

