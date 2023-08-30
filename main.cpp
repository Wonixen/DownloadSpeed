#include <iostream>
#include <format>

#include <windows.h>
#include <Urlmon.h>

#include <string>
#include <algorithm>
#include <chrono>

#pragma comment(lib, "urlmon.lib")


struct MyCallback : IBindStatusCallback
{
   HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 1;
   }

   ULONG STDMETHODCALLTYPE AddRef(void) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 1;
   }

   ULONG STDMETHODCALLTYPE Release(void) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 1;
   }

   HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD dwReserved, __RPC__in_opt IBinding* pib) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 0;
   }
   HRESULT STDMETHODCALLTYPE GetPriority(__RPC__out LONG* pnPriority) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 0;
   }

   HRESULT STDMETHODCALLTYPE OnLowResource(DWORD reserved) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 0;
   }
   HRESULT STDMETHODCALLTYPE OnProgress(ULONG ulProgress, ULONG ulProgressMax, ULONG ulStatusCode, __RPC__in_opt LPCWSTR text) final
   {
      std::string msg;
      if (text)
         std::transform(text,
                        text + wcslen(text),
                        std::back_inserter(msg),
                        [](WCHAR c)
                        {
                           return static_cast<char>(c);
                        });

      static std::string url {};

      static std::chrono::steady_clock::time_point start;
      if (url != msg)
      {
         start = std::chrono::steady_clock::now();
         url   = msg;
      }


      auto end = std::chrono::steady_clock::now();


      double percent = ulProgress * 100LL;
      if (ulProgressMax > 0)
         percent /= ulProgressMax;

      double mbps         = {};
      auto   milliElapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
      if (milliElapsed > 0)
         mbps = (ulProgress * (1000000.0 / milliElapsed) * 8.0) / (1024.0 * 1024.0);

      std::cout << std::format("{:10.5f} {:10.5f} {} \n", percent, mbps, milliElapsed);
      return 0;
   }

   HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT hresult, __RPC__in_opt LPCWSTR szError) final
   {
      std::cout << std::format("{}({})", __FUNCSIG__, hresult) << std::endl;
      return 0;
   }

   HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD* grfBINDF, BINDINFO* pbindinfo) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 0;
   }

   HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD grfBSCF, DWORD dwSize, FORMATETC* pformatetc, STGMEDIUM* pstgmed) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 0;
   }

   HRESULT STDMETHODCALLTYPE OnObjectAvailable(__RPC__in REFIID riid, __RPC__in_opt IUnknown* punk) final
   {
      std::cout << std::format("{}", __FUNCSIG__) << std::endl;
      return 0;
   }
};

int main(int argc, char* argv[])
{
   const char* url = "https://youtu.be/bgkQ8ITkGkE?list=RDbgkQ8ITkGkE";

   if (argc > 1)
      url = argv[1];

   char path[MAX_PATH];
   GetCurrentDirectory(MAX_PATH, path);
   strcat(path, "\\test.mpg");

   std::cout << std::format("downloading: {}, url to {}", url, path) << std::endl;
   MyCallback cb;
   HRESULT    res = URLDownloadToFile(nullptr, url, path, 0, &cb);

   if (res == S_OK)
   {
      printf("Ok\n");
   }
   else if (res == E_OUTOFMEMORY)
   {
      printf("Buffer length invalid, or insufficient memory\n");
   }
   else if (res == INET_E_DOWNLOAD_FAILURE)
   {
      printf("URL is invalid\n");
   }
   else
   {
      printf("Other error: %x, %d\n", res, res);
   }

   return 0;
}