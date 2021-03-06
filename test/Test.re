open FileReader;
open Expect;
open Belt;

let isEdge: bool = [%raw {|
(navigator.appVersion.indexOf("Edge") != -1)
|}];

// test name clash
module File = FileReader.File;

let fr = FileReader.make();

expectToEqual(fr->error, None);

fr->onload(e => {
  Js.log2("onload e", e);

  expectToEqual(fr->result, Some(`String({js|АБ|js})));
});

fr->onerror(e => {
  Js.log2("onerror e", e);
  failwith("unreacahble");
});

open Js.Promise;
open Js.Typed_array;

let now = Js.Date.make()->Js.Date.getTime;

if (!isEdge) {
  let opts = File.Options.make(~_type="my/type", ~lastModified=now, ());
  let file =
    File.make3(
      [|BlobPart.uint8Array(Uint8Array.make([|192, 193|]))|],
      "file1",
      opts,
    );
  expectToEqual(file->File.name, "file1");
  expectToEqual(file->File.lastModified->Js.typeof, "number");
  expectToEqual(file->File.size->Js.typeof, "number");
  expectToEqual(file->File.lastModified, now);
  //   expectToEqual(file->File.type_, "");
  expectToEqual(file->File.type_, "my/type");
  expectToEqual(file->File.size, 2.0);

  /* cast there and back */
  expectToEqual(
    file->File.asBlob->Blob.asFile->Option.getExn->File.name,
    "file1",
  );
};

let blob =
  Blob.make2(
    [|BlobPart.uint8Array(Uint8Array.make([|65, 66, 67|]))|],
    Blob.Options.make(~_type="old/type", ()),
  );
expectToEqual(blob->Blob.type_, "old/type");
expectToEqual(blob->Blob.asFile->Option.isNone, true);

let sliced = blob->Blob.slice3(~start=1, ~end_=3, ~contentType="new/type");
expectToEqual(sliced->Blob.type_, "new/type");
expectToEqual(sliced->Blob.size, 2.0);

sliced->toArrayBuffer
|> then_(ab => {
     let arr = Uint8Array.fromBuffer(ab);
     expectToEqual(arr->Uint8Array.byteLength, 2);
     expectToEqual(arr->Uint8Array.unsafe_get(0), 66);
     expectToEqual(arr->Uint8Array.unsafe_get(1), 67);
     resolve();
   });

// should not compile
//Js.log2("blob name", blob->File.name);
//file->Blob.toDataURL;

let file =
  Blob.make2(
    [|BlobPart.uint8Array(Uint8Array.make([|192, 193|]))|],
    Blob.Options.make(~_type="my/type", ()),
  );

//fr->readAsArrayBuffer(file->File.asBlob);
//fr->readAsBinaryString(file->File.asBlob);
fr->readAsText2(file, ~encoding="Windows-1251");
//fr->readAsDataURL(file->File.asBlob);

//fr->readAsText(blob);

toArrayBuffer(blob)
|> then_(ab => {
     expectInstanceOf(ab, array_buffer_ctor);
     resolve();
   });

toDataURL(blob)
|> then_(s => {
     expectToEqual(s->Js.typeof, "string");
     resolve();
   });

toText2(file, ~encoding="Windows-1251")
|> then_(s => {
     expectToEqual(s, {js|АБ|js});
     resolve();
   });

let blob2 =
  Blob.make([|BlobPart.string("hello"), BlobPart.string(" world")|]);
blob2->toText
|> then_(s => {
     expectToEqual(s, "hello world");
     resolve();
   });

Js.log("OK");