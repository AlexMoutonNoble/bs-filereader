type t = FileReader__.file;

include (module type of
  FileReader_BlobLike.Make({
    type nonrec t = t;
  }));

[@bs.get] external name: t => string = "name";
[@bs.get] external lastModified: t => float = "lastModified";

external asBlob: t => FileReader_Blob.t = "%identity";

let make:
  (
    array(FileReader__.blobPart),
    string,
    ~type_: string=?,
    ~lastModified: float=?,
    unit
  ) =>
  t;
