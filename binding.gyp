{
  "targets": [
    {
      "target_name": "temporal_tick",
      "sources": ["temporal-tick.cc"],
      "include_dirs": [
        '<!(node -e "require(\'nan\')")'
      ],
    }
  ]
}
