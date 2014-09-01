/*******************************************************************
 * HTMLDecorator.h
 *
 *  @date: 14-8-2014
 *  @author: DB
 ******************************************************************/

#ifndef HTMLDECORATOR_H_
#define HTMLDECORATOR_H_

#include <stdio.h>
#include <string>

#include <builder/Templater.h>
#include <common/traceout.h>
#include <common/Utils.h>

//---------------------------------------------------------------------------------------
template<typename T>
class HTMLDecorator
//---------------------------------------------------------------------------------------
{
   public:
      HTMLDecorator();
      virtual ~HTMLDecorator();

      void setURL(const std::string& URL);

      std::string decorateLine  (T line);
      std::string decorateBody  (const std::string& body);
      std::string decorateHeader(const std::string& title);
      std::string decorateFooter(const std::string& footer);
      std::string decoratePage  (const std::string& header,
                                 const std::string& body,
                                 const std::string& footer);

   private:
      void escapeSpecialEntities(std::string& text);

      std::string mURL;
};

//---------------------------------------------------------------------------------------
template<typename T>
HTMLDecorator<T>::HTMLDecorator(){}

//---------------------------------------------------------------------------------------
template<typename T>
HTMLDecorator<T>::~HTMLDecorator(){}

//---------------------------------------------------------------------------------------
template<typename T>
void HTMLDecorator<T>::setURL(const std::string& URL)
//---------------------------------------------------------------------------------------
{
   mURL = URL;
}

//---------------------------------------------------------------------------------------
template<typename T>
void HTMLDecorator<T>::escapeSpecialEntities(std::string& text)
//---------------------------------------------------------------------------------------
{
   Utils::replaceAll(text, "<", "&lt;");
   Utils::replaceAll(text, ">", "&gt;");
   Utils::replaceAll(text, "\"", "&quot;");
   Utils::replaceAll(text, "�", "&euro;");
}

//---------------------------------------------------------------------------------------
template<typename T>
std::string HTMLDecorator<T>::decoratePage(  const std::string& header,
                                             const std::string& body,
                                             const std::string& footer)
//---------------------------------------------------------------------------------------
{
   TRC_DEBUG_FUNC_ENTER(0U, "header='%s'", header.c_str());

   Templater templater(Templater::PATH_ROOT_LAYOUT);

   templater.setMacro(Templater::MACROS_ROOT,      mURL);
   templater.setMacro(Templater::MACROS_HEADER,    header);
   templater.setMacro(Templater::MACROS_CONTENT,   body);
   templater.setMacro(Templater::MACROS_FOOTER,    footer);

   std::string output = templater.generate();

   TRC_DEBUG_FUNC_EXIT (0U);

   return output;
}

// to trivial to templatize
//---------------------------------------------------------------------------------------
template<typename T>
//---------------------------------------------------------------------------------------
std::string HTMLDecorator<T>::decorateHeader(const std::string& title)
{
   return "<title>"+title+": "+ mURL +" </title>";
}

//---------------------------------------------------------------------------------------
template<typename T>
//---------------------------------------------------------------------------------------
std::string HTMLDecorator<T>::decorateFooter(const std::string& footer)
{
   return "<span class=\"tag\"> " + footer + " </span>";
}

#endif /* HTMLDECORATOR_H_ */
