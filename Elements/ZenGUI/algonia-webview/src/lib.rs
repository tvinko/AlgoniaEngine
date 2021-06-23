use std::ffi::CStr;
use std::fs;
use std::os::raw::c_char;
use web_view::*;

extern crate web_view;
use select::predicate::*;

pub use select::document::Document;

#[no_mangle]
pub extern "C" fn init_web_view(
  algonia_callback: unsafe extern "C" fn(s_arg: *const c_char) -> *const c_char,
  algonia_free: unsafe extern "C" fn(),
  s_html_file_path: *const c_char,
) {
  let c_str = unsafe { CStr::from_ptr(s_html_file_path) };
  let mut index_file = c_str.to_str().unwrap().to_string();
  index_file.push_str("/index.html");

  let mut html = fs::read_to_string(index_file).expect("Something went wrong reading the file");

  let document = Document::from(&html[..]);
  for node in document.select(Name("algonia_html")) {
    let html_content =
      fs::read_to_string(node.attr("src").unwrap()).expect("Something went wrong reading the file");
    html = html.replace(&node.html()[..], &html_content);
  }

  for node in document.select(Name("algonia_script")) {
    let mut html_content = "<script type='text/javascript' charset='utf-8'>".to_owned();
    if (node.attr("src").unwrap() == "system") {
      html_content.push_str(ALGONIA_SCRIPTS);
    } else {
      html_content.push_str(
        &(fs::read_to_string(node.attr("src").unwrap())
          .expect("Something went wrong reading the file")),
      );
    }
    html_content.push_str("</script>");
    html = html.replace(&node.html()[..], &html_content);
  }

  for node in document.select(Name("algonia_style")) {
    let mut html_content = "<style type='text/css'>".to_owned();
    let file_content =
      fs::read_to_string(node.attr("src").unwrap()).expect("Something went wrong reading the file");
    html_content.push_str(&file_content);
    html_content.push_str("</style>");
    html = html.replace(&node.html()[..], &html_content);
  }

  let webview = web_view::builder()
    .title("Fullscreen example")
    .content(Content::Html(html))
    .size(800, 800)
    .resizable(true)
    .debug(true)
    .user_data(0)
    .invoke_handler(|webview, arg| {
      unsafe {
        let algonia_result = CStr::from_ptr(algonia_callback(arg.as_ptr() as *const c_char))
          .to_str()
          .unwrap()
          .to_string();
        algonia_free();
        let parsed = json::parse(arg).unwrap();
        webview.eval(&format!(
          "{}('{}')",
          parsed["successCallback"], algonia_result
        ));
      }

      Ok(())
    })
    .build()
    .unwrap();

  webview.run().unwrap();
}

const ALGONIA_SCRIPTS: &str = r#"
  function alg_execute(nodeId, successCallback) {
    external.invoke(JSON.stringify({ cmd: 'exec', nodeId: nodeId, successCallback:successCallback  }));
  }

  function alg_execute_and_get_result(nodeId, successCallback) {
    external.invoke(JSON.stringify({ cmd: 'exec_get_result', nodeId: nodeId, successCallback:successCallback  }));
  }

  function alg_get_result(nodeId, successCallback) {
    external.invoke(JSON.stringify({ cmd: 'get_result', nodeId: nodeId, successCallback:successCallback  }));
  }

  function alg_set_arg(nodeId, val, successCallback) {
    external.invoke(JSON.stringify({ cmd: 'set_arg', nodeId: nodeId, val: val, successCallback:successCallback  }));
  }

  function alg_set_arg_and_execute(nodeId, val, successCallback) {
    external.invoke(JSON.stringify({ cmd: 'set_arg_exec', nodeId: nodeId, val: val, successCallback:successCallback  }));
  }

  function alg_set_arg_execute_and_get_result(nodeId, val, successCallback) {
    external.invoke(JSON.stringify({ cmd: 'set_arg_exec_get_result', nodeId: nodeId, val: val, successCallback:successCallback  }));
  }

  function alg_get_arg(nodeId, successCallback) {
    external.invoke(JSON.stringify({ cmd: 'get_arg', nodeId: nodeId, successCallback:successCallback  }));
  }
"#;
