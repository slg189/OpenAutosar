# manifests — 信息安全分级视图

通过 google-repo manifest 控制不同角色可见的源码范围 (`repo_metasar.sh` 使用)。

| Manifest | 角色 | 源码可见 | 二进制 (.a + inc) |
|----------|------|----------|-------------------|
| `default.xml` | 平台/集成负责人 | 全部 | — |
| `manifest-asw.xml` | ASW 工程师 | `ASWs/`、`Components/`、`Projects/` | `BSW_Libs`、`CDD_Libs`、`MCAL_Libs` |
| `manifest-integration.xml` | 基础(集成)工程师 | `Projects/*/Integration` | `ASW_Libs`、`BSW_Libs`、`CDD_Libs`、`MCAL_Libs` |

切换视图:
```bash
MANIFEST_NAME=manifest-asw.xml ./repo_metasar.sh init
./repo_metasar.sh sync
```
