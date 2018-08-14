# AiHttpHook
使用APIHOOK技术对Wininet.dll的相关函数进行Hook用于拦截当前进程的Http请求，应用于ToukenBrowser的3.5及更早的版本。

Use APIHook technology to perform Hooks on Wininet.dll functions(unicode) to intercept Http and https requests for the current process (x86 and x64).

vs2017 build

Quick Start (C#)
```csharp
    class AiHttpHook64
    {
        [DllImport("AiHttpHook64.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int StartHook(IntPtr callback);
    }

    class AiHttpHook
    {
        [DllImport("AiHttpHook.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        public static extern int StartHook(IntPtr callback);
    }

    class Hook
    {
        private delegate int HttpCallback(string path, 
                                          string responseData, 
                                          string postData, 
                                          string hostName, 
                                          string responseHeaders);

        private static HttpCallback HttpPackCallback;

        /// <summary>
        /// Initialization function
        /// use this func first.
        /// </summary>
        public static void Init()
        {
            HttpPackCallback = new HttpCallback(DataRoute);
            if (Environment.Is64BitProcess)
            {
                //64-bit
                int ret = AiHttpHook64.StartHook(Marshal.GetFunctionPointerForDelegate(HttpPackCallback));
            }
            else
            {
                int ret = AiHttpHook.StartHook(Marshal.GetFunctionPointerForDelegate(HttpPackCallback));
            }
        }


        /// <summary>
        /// callback
        /// </summary>
        /// <param name="path">path</param>
        /// <param name="responseData">responseData</param>
        /// <param name="postData">postData</param>
        /// <param name="hostName">hostName</param>
        /// <param name="responseHeaders">responseHeaders</param>
        /// <returns></returns>
        public static int DataRoute(string path, 
                                    string responseData, 
                                    string postData, 
                                    string hostName, 
                                    string responseHeaders)
        {
            // use data 
            return 0;
        }
    }
    
```

